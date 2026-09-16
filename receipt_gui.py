import json
from datetime import datetime
from pathlib import Path
import tkinter as tk
from tkinter import messagebox, ttk

DATA_FILE = Path(__file__).with_name("receipt.json")
FEE_HEADS = [
    "Form Fees",
    "Semester Fees",
    "Admission Fees",
    "Caution Money",
    "Security",
    "Medical",
    "Annual Fees",
    "Examination Fees",
    "Bus Fees",
    "Hostel Fees",
]


class ReceiptGUI:
    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.title("Receipt Management System")
        self.root.geometry("1320x760")

        self.receipts = self.load_receipts()
        self.selected_receipt_no = None

        self.fields = {}
        self.fee_entries = {}

        self.build_form()
        self.build_table()
        self.refresh_table(self.receipts)

    def load_receipts(self):
        if not DATA_FILE.exists():
            return []
        try:
            with DATA_FILE.open("r", encoding="utf-8") as file:
                data = json.load(file)
                return data if isinstance(data, list) else []
        except (json.JSONDecodeError, OSError):
            return []

    def save_receipts(self):
        with DATA_FILE.open("w", encoding="utf-8") as file:
            json.dump(self.receipts, file, indent=2)

    def build_form(self):
        main = ttk.Frame(self.root, padding=10)
        main.pack(fill="both", expand=True)

        form = ttk.LabelFrame(main, text="Receipt Details", padding=10)
        form.pack(side="left", fill="y")

        base_fields = [
            ("receipt_no", "Receipt No"),
            ("date", "Date (DD-MM-YYYY)"),
            ("student_name", "Student Name"),
            ("course", "Course"),
            ("semester", "Semester"),
            ("upi_ref", "UPI Ref No"),
            ("account_officer", "Account Officer"),
        ]

        for row, (key, label) in enumerate(base_fields):
            ttk.Label(form, text=label).grid(row=row, column=0, sticky="w", padx=4, pady=4)
            entry = ttk.Entry(form, width=34)
            entry.grid(row=row, column=1, padx=4, pady=4)
            self.fields[key] = entry

        ttk.Label(form, text="Payment Mode").grid(row=7, column=0, sticky="w", padx=4, pady=4)
        payment = ttk.Combobox(form, state="readonly", values=["Cash", "Online"], width=31)
        payment.set("Cash")
        payment.grid(row=7, column=1, padx=4, pady=4)
        self.fields["payment_mode"] = payment

        fee_frame = ttk.LabelFrame(form, text="Fee Heads", padding=8)
        fee_frame.grid(row=8, column=0, columnspan=2, sticky="ew", padx=4, pady=8)

        for index, head in enumerate(FEE_HEADS):
            ttk.Label(fee_frame, text=head).grid(row=index, column=0, sticky="w", padx=4, pady=2)
            entry = ttk.Entry(fee_frame, width=16)
            entry.insert(0, "0")
            entry.grid(row=index, column=1, padx=4, pady=2)
            self.fee_entries[head] = entry

        button_frame = ttk.Frame(form)
        button_frame.grid(row=9, column=0, columnspan=2, pady=12)

        ttk.Button(button_frame, text="Add", command=self.add_receipt).grid(row=0, column=0, padx=3)
        ttk.Button(button_frame, text="Update Selected", command=self.update_receipt).grid(row=0, column=1, padx=3)
        ttk.Button(button_frame, text="Delete Selected", command=self.delete_receipt).grid(row=0, column=2, padx=3)
        ttk.Button(button_frame, text="Clear", command=self.clear_form).grid(row=0, column=3, padx=3)

        search_frame = ttk.LabelFrame(form, text="Search", padding=8)
        search_frame.grid(row=10, column=0, columnspan=2, sticky="ew", padx=4, pady=4)

        self.search_key = ttk.Combobox(
            search_frame,
            state="readonly",
            values=[
                "receipt_no",
                "student_name",
                "date",
                "month",
                "year",
                "course",
                "semester",
                "payment_mode",
                "account_officer",
            ],
            width=15,
        )
        self.search_key.set("receipt_no")
        self.search_key.grid(row=0, column=0, padx=4, pady=3)

        self.search_value = ttk.Entry(search_frame, width=16)
        self.search_value.grid(row=0, column=1, padx=4, pady=3)

        ttk.Button(search_frame, text="Search", command=self.search_receipts).grid(row=0, column=2, padx=4)
        ttk.Button(search_frame, text="Reset", command=lambda: self.refresh_table(self.receipts)).grid(row=0, column=3, padx=4)

    def build_table(self):
        container = ttk.Frame(self.root, padding=(0, 10, 10, 10))
        container.place(relx=0.38, rely=0.0, relwidth=0.62, relheight=1.0)

        columns = (
            "receipt_no",
            "date",
            "student_name",
            "course",
            "semester",
            "payment_mode",
            "total_fee",
            "account_officer",
        )

        self.table = ttk.Treeview(container, columns=columns, show="headings", height=31)
        for col in columns:
            self.table.heading(col, text=col.replace("_", " ").title())
            self.table.column(col, anchor="center", width=120)

        self.table.column("student_name", width=180)
        self.table.column("account_officer", width=180)

        scrollbar = ttk.Scrollbar(container, orient="vertical", command=self.table.yview)
        self.table.configure(yscrollcommand=scrollbar.set)

        self.table.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")

        self.table.bind("<<TreeviewSelect>>", self.on_row_select)

    def clear_form(self):
        for key, widget in self.fields.items():
            if key == "payment_mode":
                widget.set("Cash")
            else:
                widget.delete(0, "end")
        for entry in self.fee_entries.values():
            entry.delete(0, "end")
            entry.insert(0, "0")
        self.selected_receipt_no = None

    def collect_form_data(self):
        try:
            receipt_no = int(self.fields["receipt_no"].get().strip())
            semester = int(self.fields["semester"].get().strip())
        except ValueError:
            messagebox.showerror("Invalid Input", "Receipt number and semester must be numeric.")
            return None

        date_text = self.fields["date"].get().strip()
        if not self.is_valid_date(date_text):
            messagebox.showerror("Invalid Date", "Date format must be DD-MM-YYYY.")
            return None

        student_name = self.fields["student_name"].get().strip().title()
        course = self.fields["course"].get().strip().upper()
        payment_mode = self.fields["payment_mode"].get().strip()
        upi_ref = self.fields["upi_ref"].get().strip()
        account_officer = self.fields["account_officer"].get().strip().title()

        if not student_name or not course or not account_officer:
            messagebox.showerror("Missing Data", "Name, course and account officer are required.")
            return None

        fees = {}
        total_fee = 0
        for head, entry in self.fee_entries.items():
            value = entry.get().strip() or "0"
            try:
                amount = int(value)
                if amount < 0:
                    raise ValueError
            except ValueError:
                messagebox.showerror("Invalid Fee", f"'{head}' must be a non-negative number.")
                return None
            fees[head] = amount
            total_fee += amount

        if payment_mode == "Online" and not upi_ref:
            messagebox.showerror("Missing Data", "UPI Ref No is required for online payment.")
            return None

        if payment_mode == "Cash":
            upi_ref = ""

        return {
            "receipt_no": receipt_no,
            "date": date_text,
            "student_name": student_name,
            "course": course,
            "semester": semester,
            "payment_mode": payment_mode,
            "upi_ref": upi_ref,
            "fees": fees,
            "total_fee": total_fee,
            "account_officer": account_officer,
        }

    @staticmethod
    def is_valid_date(date_text):
        try:
            datetime.strptime(date_text, "%d-%m-%Y")
            return True
        except ValueError:
            return False

    def refresh_table(self, rows):
        self.table.delete(*self.table.get_children())
        for receipt in rows:
            self.table.insert(
                "",
                "end",
                iid=str(receipt["receipt_no"]),
                values=(
                    receipt["receipt_no"],
                    receipt["date"],
                    receipt["student_name"],
                    receipt["course"],
                    receipt["semester"],
                    receipt["payment_mode"],
                    receipt["total_fee"],
                    receipt["account_officer"],
                ),
            )

    def add_receipt(self):
        data = self.collect_form_data()
        if not data:
            return

        if any(item["receipt_no"] == data["receipt_no"] for item in self.receipts):
            messagebox.showerror("Duplicate", "Receipt number already exists.")
            return

        self.receipts.append(data)
        self.save_receipts()
        self.refresh_table(self.receipts)
        self.clear_form()
        messagebox.showinfo("Success", "Receipt added successfully.")

    def update_receipt(self):
        if self.selected_receipt_no is None:
            messagebox.showwarning("Select Row", "Select a receipt from the table first.")
            return

        data = self.collect_form_data()
        if not data:
            return

        if data["receipt_no"] != self.selected_receipt_no and any(
            item["receipt_no"] == data["receipt_no"] for item in self.receipts
        ):
            messagebox.showerror("Duplicate", "New receipt number already exists.")
            return

        for idx, receipt in enumerate(self.receipts):
            if receipt["receipt_no"] == self.selected_receipt_no:
                self.receipts[idx] = data
                break

        self.save_receipts()
        self.refresh_table(self.receipts)
        self.clear_form()
        messagebox.showinfo("Success", "Receipt updated successfully.")

    def delete_receipt(self):
        if self.selected_receipt_no is None:
            messagebox.showwarning("Select Row", "Select a receipt from the table first.")
            return

        if not messagebox.askyesno("Confirm", "Delete selected receipt?"):
            return

        self.receipts = [r for r in self.receipts if r["receipt_no"] != self.selected_receipt_no]
        self.save_receipts()
        self.refresh_table(self.receipts)
        self.clear_form()
        messagebox.showinfo("Success", "Receipt deleted successfully.")

    def on_row_select(self, _event):
        selected = self.table.selection()
        if not selected:
            return

        receipt_no = int(selected[0])
        receipt = next((r for r in self.receipts if r["receipt_no"] == receipt_no), None)
        if not receipt:
            return

        self.selected_receipt_no = receipt_no

        self.fields["receipt_no"].delete(0, "end")
        self.fields["receipt_no"].insert(0, receipt["receipt_no"])

        self.fields["date"].delete(0, "end")
        self.fields["date"].insert(0, receipt["date"])

        self.fields["student_name"].delete(0, "end")
        self.fields["student_name"].insert(0, receipt["student_name"])

        self.fields["course"].delete(0, "end")
        self.fields["course"].insert(0, receipt["course"])

        self.fields["semester"].delete(0, "end")
        self.fields["semester"].insert(0, receipt["semester"])

        self.fields["payment_mode"].set(receipt["payment_mode"])

        self.fields["upi_ref"].delete(0, "end")
        self.fields["upi_ref"].insert(0, receipt["upi_ref"])

        self.fields["account_officer"].delete(0, "end")
        self.fields["account_officer"].insert(0, receipt["account_officer"])

        for head, entry in self.fee_entries.items():
            entry.delete(0, "end")
            entry.insert(0, receipt["fees"].get(head, 0))

    def search_receipts(self):
        key = self.search_key.get()
        value = self.search_value.get().strip()
        if not value:
            messagebox.showwarning("Missing Value", "Enter a value to search.")
            return

        value_lower = value.lower()
        results = []

        for receipt in self.receipts:
            if key == "receipt_no" and str(receipt["receipt_no"]) == value:
                results.append(receipt)
            elif key == "student_name" and value_lower in receipt["student_name"].lower():
                results.append(receipt)
            elif key == "date" and receipt["date"] == value:
                results.append(receipt)
            elif key == "month":
                month = receipt["date"].split("-")[1]
                if month == value.zfill(2):
                    results.append(receipt)
            elif key == "year":
                year = receipt["date"].split("-")[2]
                if year == value:
                    results.append(receipt)
            elif key == "course" and value_lower in receipt["course"].lower():
                results.append(receipt)
            elif key == "semester" and str(receipt["semester"]) == value:
                results.append(receipt)
            elif key == "payment_mode" and value_lower == receipt["payment_mode"].lower():
                results.append(receipt)
            elif key == "account_officer" and value_lower in receipt["account_officer"].lower():
                results.append(receipt)

        self.refresh_table(results)
        if not results:
            messagebox.showinfo("No Results", "No matching receipts found.")


if __name__ == "__main__":
    root = tk.Tk()
    app = ReceiptGUI(root)
    root.mainloop()

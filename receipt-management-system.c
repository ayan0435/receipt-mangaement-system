#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#define FILENAME "receipt.dat"

typedef struct {
    int receiptNo, date, mon, yr;
    char name[100];
    char course[20];
    int sem, pay;
    long long int upi;
    int fee[10];
    long long int totalFee;
    char accOff[100];
} receipt;

void addFee();
void viewAllReceipts();
void searchReceipt();
void updateReceipt();
void deleteReceipt();
int displayReceipt(receipt s);
int match(receipt s, int n, char *value);
int matchDate(receipt s, int date, int mon, int yr);
int matchMon(receipt s, int mon, int yr);
void capitaliseName(char a[]);
void capitaliseWord(char a[]);
void convertNumToFigure(long long int num);
void printNumInWord(int num);
int isValidDate(int d, int m, int y);

int main() {
    int choice;
    do {
        printf("\n---Receipt Management System---\n\n");
        printf("1. Add Fee\n2. View All Receipts\n3. Search Receipt\n4. Update Receipt \n5. Delete Receipt \n6. Exit\nEnter Choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("________________________________________\n");
            printf("\nInvalid Choice!!\nPlease enter a number between 1 and 6.\n");
            printf("________________________________________\n");
            while (getchar() != '\n');
            continue;
        }
        switch(choice) {
        case 1:
            addFee();
            break;
        case 2:
            viewAllReceipts();
            break;
        case 3:
            searchReceipt();
            break;
        case 4:
            updateReceipt();
            break;
        case 5:
            deleteReceipt();
            break;
        case 6:
            printf("________________________________________\n");
            printf("\nExiting...\n");
            printf("________________________________________\n");
            break;
        default:
            printf("________________________________________\n");
            printf("\nInvalid Choice!!\nPlease enter a number between 1 and 6.\n");
            printf("________________________________________\n");
        }
    } while(choice!=6);
    return 0;
}

void addFee() {
    FILE *fptr=fopen(FILENAME, "ab");
    if(fptr==NULL) {
        printf("\nError in opening file!!\n");
        return;
    }
    receipt s;

    s.totalFee=0;
    for(int i=0; i<10; i++)
        s.fee[i]=0;

    printf("________________________________________\n");
    printf("\n---Receipt Generator---\n");
    printf("________________________________________\n");
    printf("\nM.P. Institute of Management & Computer Application\nBhagatua, Chiraigaon, Varanasi\n\nEnter Receipt No: ");
    scanf("%d", &s.receiptNo);

    do {
        printf("Date(DD MM YYYY): ");
        scanf("%d %d %d", &s.date, &s.mon, &s.yr);
    } while(!isValidDate(s.date, s.mon, s.yr));

    getchar();
    printf("Student Name: ");
    fgets(s.name, 100, stdin);
    capitaliseName(s.name);
    printf("Course: ");
    fgets(s.course, 20, stdin);
    s.course[strcspn(s.course, "\n")] = '\0';
    capitaliseWord(s.course);
    printf("Semester: ");
    scanf("%d", &s.sem);
    printf("Mode of Payment(Enter 0=Cash or 1=Online): ");
    scanf("%d", &s.pay);
    if(s.pay) {
        printf("UPI Ref No: ");
        scanf("%lld", &s.upi);
    }

    int feeNo, n;
    do {
        printf("\n---Which Fee---\n1. Form Fees\n2. Semester Fees\n3. Admission Fees\n4. Caution Money(Refundable)\n5. Security\n6. Medical\n7. Annual Fees\n8. Examination Fees\n9. Bus Fees\n10. Hostel Fees\n\nEnter numeric value only: ");
        scanf("%d", &feeNo);
        if(feeNo<1 || feeNo>10) {
            printf("\nInvalid Fee No!! Enter Again\n");
            n=1;
            continue;
        }
        printf("Enter amount: ");
        scanf("%d", &s.fee[feeNo-1]);
        s.totalFee += s.fee[feeNo-1];
        printf("For other fees enter 1 otherwise 0: ");
        scanf("%d", &n);
    } while(n);
    getchar();
    printf("Account Officer: ");
    fgets(s.accOff, 100, stdin);
    capitaliseName(s.accOff);

    fwrite(&s, sizeof(receipt), 1, fptr);
    fclose(fptr);

    displayReceipt(s);
    printf("\nReceipt Generated Successfully✓\n");
    printf("________________________________________\n");
}

void viewAllReceipts() {
    FILE *fptr = fopen(FILENAME, "rb");
    if(fptr==NULL) {
        printf("\nError in opening file!!\n");
        return;
    }
    receipt s;

    long long int finalAmt=0;

    printf("\n________________________________________\n");
    printf("\n---All Receipts---\n");
    printf("________________________________________\n");

    while(fread(&s, sizeof(receipt), 1, fptr))
        finalAmt+=displayReceipt(s);

    if(finalAmt) {
        printf("\nTotal Collected Fee: %lld INR\n", finalAmt);
        convertNumToFigure(finalAmt);
        printf("\nReceipt Fetched Successfuly...✓\n");
    }
    else {
        printf("________________________________________\n");
        printf("\nNo Records Found!!\n");
    }

    printf("________________________________________\n");
    fclose(fptr);
}

void searchReceipt() {
    FILE *fptr=fopen(FILENAME, "rb");
    if(fptr==NULL) {
        printf("\nError in opening file!!\n");
        return;
    }
    receipt s;

    long long int totalAmt=0;

    int n, found=0;
    char value[100];
    int date, mon, yr;


    printf("________________________________________\n");
    printf("\nSearch By:\n1. Receipt No\n2. Specific Date\n3. Specific Month\n4. Specific Year\n5. Student Name\n6. Course\n7. Semester\n8. Mode of Payment\n9. Account Officer\nEnter Choice: ");
    scanf("%d", &n);

    if(n<1 || n>9) {
        printf("________________________________________\n");
        printf("\nInvalid Choice!!\n");
        printf("________________________________________\n");
        return;
    }

    getchar();
    if(n==2) {
        printf("Enter Date(DD MM YYYY): ");
        scanf("%d %d %d", &date, &mon, &yr);
    }
    else if(n==3) {
        printf("Enter Month(MM YYYY): ");
        scanf("%d %d", &mon, &yr);
    }
    else {
        printf("Enter Value: ");
        fgets(value, sizeof(value), stdin);
        value[strcspn(value, "\n")]='\0';
    }

    while(fread(&s, sizeof(receipt), 1, fptr)) {
        if(n==2) {
            if(matchDate(s, date, mon, yr)) {
                displayReceipt(s);
                totalAmt+=s.totalFee;
                found=1;
            }
        }
        else if(n==3) {
            if(matchMon(s, mon, yr)) {
                displayReceipt(s);
                totalAmt+=s.totalFee;
                found=1;
            }
        }
        else {
            if(match(s, n, value)) {
                displayReceipt(s);
                totalAmt+=s.totalFee;
                found=1;
            }
        }
    }

    if(!found) {
        printf("________________________________________\n");
        printf("\nNo Records Found!!\n");
        printf("________________________________________\n");
    }
    else {
        printf("\nTotal Fee Collected - %lld INR\n", totalAmt);
        convertNumToFigure(totalAmt);
        printf("\nReceipt Fetched Succesfully...✓\n");
        printf("________________________________________\n");
    }

    fclose(fptr);
}

void updateReceipt() {
    FILE *fptr=fopen(FILENAME, "rb+");
    if(fptr==NULL) {
        printf("\nError in opening file!!\n");
        return;
    }
    receipt s;

    int found=0;
    int value, n=1;

    printf("Receipt No: ");
    scanf("%d", &value);

    while(fread(&s, sizeof(receipt), 1, fptr)) {
        if(value==s.receiptNo) {
            printf("\nReceipt found enter new details-\nReceipt No: %d", value);
            int checkDate=1;
            do {
                printf("\nDate(DD MM YYYY): ");
                scanf("%d %d %d", &s.date, &s.mon, &s.yr);
            } while(!isValidDate(s.date, s.mon, s.yr));

            getchar();
            printf("Student Name: ");
            fgets(s.name, 100, stdin);
            capitaliseName(s.name);
            printf("Course: ");
            fgets(s.course, 20, stdin);
            s.course[strcspn(s.course, "\n")] = '\0';
            capitaliseWord(s.course);
            printf("Semester: ");
            scanf("%d", &s.sem);
            printf("Mode of Payment(Enter 0=Cash or 1=Online): ");
            scanf("%d", &s.pay);
            if(s.pay) {
                printf("UPI Ref No: ");
                scanf("%lld", &s.upi);
            }

            int feeNo, n;
            s.totalFee=0;
            for(int i=0; i<10; i++)
                s.fee[i]=0;
            do {
                printf("\n---Which Fee---\n1. Form Fees\n2. Semester Fees\n3. Admission Fees\n4. Caution Money(Refundable)\n5. Security\n6. Medical\n7. Annual Fees\n8. Examination Fees\n9. Bus Fees\n10. Hostel Fees\n\nEnter numeric value only: ");
                scanf("%d", &feeNo);
                if(feeNo<1 || feeNo>10) {
                    printf("\nInvalid Fee No!! Enter Again\n");
                    n=1;
                    continue;
                }
                printf("Enter amount: ");
                scanf("%d", &s.fee[feeNo-1]);
                s.totalFee += s.fee[feeNo-1];
                printf("For other fees enter 1 otherwise 0: ");
                scanf("%d", &n);
            } while(n);
            getchar();
            printf("Account Officer: ");
            fgets(s.accOff, 100, stdin);
            capitaliseName(s.accOff);

            fseek(fptr, -sizeof(receipt), SEEK_CUR);
            fwrite(&s, sizeof(receipt), 1, fptr);
            found=1;
            break;
        }
    }

    if(found) {
        displayReceipt(s);
        printf("\nReceipt Updated Successfully✓");
        printf("\n________________________________________\n");
    }
    else {
        printf("________________________________________\n");
        printf("\nNo Records Found!!\n");
        printf("________________________________________\n");
    }

    fclose(fptr);
}

void deleteReceipt() {
    FILE *fptr=fopen(FILENAME, "rb");
    FILE *temp=fopen("temp.dat", "wb");
    if(fptr==NULL || temp==NULL) {
        printf("\nError in opening file!!\n");
        return;
    }
    receipt s;

    int found=0;
    int value;
    char confirm;

    printf("Receipt No: ");
    scanf("%d", &value);
    getchar();

    while(fread(&s, sizeof(receipt), 1, fptr)) {
        if(value==s.receiptNo) {
            printf("Are you sure you want to delete this record? (y/n): ");
            scanf("%c", &confirm);
            if (confirm != 'y' && confirm != 'Y') {
                fclose(fptr);
                fclose(temp);
                printf("________________________________________\n");
                printf("\nDeletion cancelled.\n");
                printf("________________________________________\n");
                return;
            }
            found=1;
        }
        else
            fwrite(&s, sizeof(receipt), 1, temp);
    }


    fclose(fptr);
    fclose(temp);

    if(found==1) {
        remove(FILENAME);
        rename("temp.dat", FILENAME);
        printf("________________________________________\n");
        printf("\nDeleted Successfully✓\n");
        printf("________________________________________\n");
    }
    else {
        printf("________________________________________\n");
        printf("\nNo Records Found!!\n");
        printf("________________________________________\n");
        remove("temp.dat");
    }
}

int displayReceipt(receipt s) {
    printf("________________________________________\n");
    printf("\nM.P. Institute of Management & Computer Application\nBhagatua, Chiraigaon, Varanasi\n");
    printf("\nReceipt No: %d\tDate: %d-%d-%d", s.receiptNo, s.date, s.mon, s.yr);
    printf("\n\nName: %sCourse: %s\t\tSemester: %d", s.name, s.course, s.sem);
    if(s.pay)
        printf("\nMode of Payment: Online (UPI: %lld)", s.upi);
    else
        printf("\nMode of Payment: Cash");
    printf("\n\nForm Fees\t\t\t| %d\nSemester Fees\t\t\t| %d\nAdmission Fees\t\t\t| %d\nCaution Money(Refundable)\t| %d\nSecurity\t\t\t| %d\nMedical\t\t\t\t| %d\nAnnual Fees\t\t\t| %d\nExamination Fees\t\t| %d\nBus Fees\t\t\t| %d\nHostel Fees\t\t\t| %d\n\n", s.fee[0], s.fee[1], s.fee[2], s.fee[3], s.fee[4], s.fee[5], s.fee[6], s.fee[7], s.fee[8], s.fee[9]);
    printf("Total Amount: %lld INR\n", s.totalFee);
    convertNumToFigure(s.totalFee);
    printf("\n\nAccount Officer - %s\nM.P. Institute of Management & Computer Application\nBhagatua, Chiraigaon, Varanasi\n", s.accOff);
    printf("________________________________________\n");
    return s.totalFee;
}

int match(receipt s, int n, char *value) {
    switch(n) {
    case 1:
        return atoi(value)==s.receiptNo;
    case 4:
        return atoi(value)==s.yr;
    case 5:
        capitaliseName(value);
        return strstr(s.name, value)!=NULL;
    case 6:
        capitaliseWord(value);
        return strstr(s.course, value)!=NULL;
    case 7:
        return atoi(value)==s.sem;
    case 8:
        return atoi(value)==s.pay;
    case 9:
        capitaliseName(value);
        return strstr(s.accOff, value)!=NULL;
    }
    return 0;
}

int matchDate(receipt s, int date, int mon, int yr) {
    if(date==s.date && mon==s.mon && yr==s.yr)
        return 1;
    else
        return 0;
}

int matchMon(receipt s, int mon, int yr) {
    if(mon==s.mon && yr==s.yr)
        return 1;
    else
        return 0;
}

void capitaliseName(char a[]) {
    int j=0;
    for(int i=0; a[i]!='\0'; i++) {
        if(i==0) a[j]=toupper(a[i]);
        if(a[i]==32) a[++j]=toupper(a[++i]);
        j++;
    }
}

void capitaliseWord(char a[]) {
    for(int i=0; a[i]!='\0'; i++)
        a[i]=toupper(a[i]);
}

void convertNumToFigure(long long int num) {
    int temp=0;

    printf("In Figure: ");
    if(num==0) {
        printf("Zero Rupee Only");
        return;
    }
    if(num>=100)
        temp=1;

    if(num>=100000000000) {
        printNumInWord(num/100000000000);
        printf("Kharab ");
        num%=100000000000;
    }
    if(num>=1000000000) {
        printNumInWord(num/1000000000);
        printf("Arab ");
        num%=1000000000;
    }
    if(num>=10000000) {
        printNumInWord(num/10000000);
        printf("Crore ");
        num%=10000000;
    }
    if(num>=100000) {
        printNumInWord(num/100000);
        printf("Lakh ");
        num%=100000;
    }
    if(num>=1000) {
        printNumInWord(num/1000);
        printf("Thousand ");
        num%=1000;
    }
    if(num>=100) {
        printNumInWord(num/100);
        printf("Hundred ");
        num%=100;
    }
    if(num>0) {
        if(temp)
            printf("And ");
        printNumInWord(num);
    }
    printf("Rupees Only");
}

void printNumInWord(int num) {
    char *ones[]= {"", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};
    char *tens[]= {"", "", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"};

    if(num>=20) {
        printf("%s ", tens[num/10]);
        if(num%10!=0)
            printf("%s ", ones[num%10]);
    }
    else
        printf("%s ", ones[num]);
}

int isValidDate(int d, int m, int y) {
    if(d<1 || d>31 || m<1 || m>12 || y<1)
        return 0;
    int mon[]= {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if((y%4==0 && y%100!=0) || (y%400==0))
        mon[1]=29;
    return d<=mon[m-1];
}
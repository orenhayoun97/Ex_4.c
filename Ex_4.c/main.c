#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINE 1023
#define NAME_SIZE 35

typedef struct StudentCourseGrade
{
    char courseName[35];
    int grade;
}StudentCourseGrade;

typedef struct Student
{
    char name[35];
    StudentCourseGrade* grades; //dynamic array of courses
    int numberOfCourses;
}Student;


//Part A
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
int countPipes(const char* lineBuffer, int maxCount);
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);
// functions not apart from the exeisce
void itoa(int n, char s[]);
void reverse(char s[]);
void freestudetns(char*** students, int* coursesPerStudent, int numberOfStudents);
void freestruct(Student* stud, int* coursesPerStudent, int numberOfStudents);

//Part B
Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents);
void writeToBinFile(const char* fileName, Student* students, int numberOfStudents);
Student* readFromBinFile(const char* fileName);

int main()
{
    //Part A
    int* coursesPerStudent = NULL;
    int numberOfStudents = 0;
    countStudentsAndCourses("//Users//orenhayoun//Desktop//שנה א׳//סמסטר ב׳//תכנות מתקדמים c//מטלות//מטלה 4//studentList.txt", &coursesPerStudent, &numberOfStudents);
    char*** students = makeStudentArrayFromFile("//Users//orenhayoun//Desktop//שנה א׳//סמסטר ב׳//תכנות מתקדמים c//מטלות//מטלה 4//studentList.txt", &coursesPerStudent, &numberOfStudents);
//    printStudentArray(students, coursesPerStudent, numberOfStudents);
    factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Linear Algebra", +5);
//    printf("\n****AFTER FACRIOR****\n");
//    printStudentArray(students, coursesPerStudent, numberOfStudents);
//    studentsToFile(students, coursesPerStudent, numberOfStudents); //this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)
    
    //Part B
    Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
    writeToBinFile("/Users/orenhayoun/Desktop/שנה א׳/סמסטר ב׳/תכנות מתקדמים c/מטלות/מטלה 4/students.bin", transformedStudents, numberOfStudents);
    Student* testReadStudents = readFromBinFile("/Users/orenhayoun/Desktop/שנה א׳/סמסטר ב׳/תכנות מתקדמים c/מטלות/מטלה 4/students.bin");
    //add code to free all arrays of struct Student
    freestruct(transformedStudents, coursesPerStudent, numberOfStudents);
    freestruct(testReadStudents, coursesPerStudent, numberOfStudents);
    // function that free the studetns
    freestudetns(students, coursesPerStudent, numberOfStudents);
    
    

    /*_CrtDumpMemoryLeaks();*/ //uncomment this block to check for heap memory allocation leaks.
    // Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

    
    return 0;
}

void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
    FILE *studentList;
    studentList = fopen(fileName,"r");
    if(studentList == NULL){
        printf("Unable to open file\n");exit(1);
    }
    char oneline[MAXLINE];
    int size = 0;
    while(fgets(oneline,MAXLINE, studentList)!=NULL){ // run until the end of file
        size++;
    }
    *coursesPerStudent = (int*) calloc(size,sizeof(int));
    if(*coursesPerStudent == NULL)
    {
        printf("allocation falid...");
        exit(1);
    }
    *numberOfStudents = size;
    rewind(studentList);
    size = 0;
    while(fgets(oneline,MAXLINE, studentList)!=NULL)
    {
        (*coursesPerStudent)[size] = countPipes(oneline,MAXLINE);
        size++;
    }
    if(fclose(studentList) != 0)
    {
        printf("Close File Fail...\n");exit(1);
    }
    return;
}

int countPipes(const char* lineBuffer, int maxCount)
{
    if(lineBuffer == NULL) return -1;
    if(maxCount <= 0) return 0;
    int counter = 0,i = 0;
    while(i < maxCount){
        if(*(lineBuffer+i) == '\0') break;
        if(*(lineBuffer+i) == '|'){
            counter++;
        }
        i++;
    }
    return counter;
}

char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
    char ***students = (char***) calloc(*numberOfStudents,sizeof(char**));
    if(students == NULL){
        printf("allocation faild...\n");
        exit(1);
    }
    FILE *studentList;
    studentList = fopen(fileName,"r");
    if(studentList == NULL){
        printf("Unable to open file\n");exit(1);
    }
    char *token = NULL;
    char oneline[MAXLINE];
    int row=0,inrow=0;

    while(fgets(oneline,MAXLINE,studentList) != NULL)
    {
        inrow = 0;
        *(students+row) = (char**) calloc((*coursesPerStudent)[row] * 2 + 1,sizeof(char*));
        if(*(students+row) == NULL){
            printf("allocation faild...\n");
            exit(1);
        }
        token = strtok(oneline,"|"); // jump in line until '|' sign
        while(token != NULL)
        {
            students[row][inrow] = (char*) malloc(strlen(token)*sizeof(char) + 1);
            if(students[row][inrow] == NULL)
            {
                printf("allocation faild...\n");
                exit(1);
            }
            strcpy(students[row][inrow], token);
            if(inrow == (*coursesPerStudent)[row] * 2)
            {
                strcpy(students[row][inrow], token);
                break;
            }
            if(inrow % 2 != 0) // once we callect the name of couce , the another we collect the score of the course
            {
                token = strtok(NULL,"|");
            }
            else token = strtok(NULL,",");
            inrow++;
        }
        row++;
    }
    if(fclose(studentList)!= 0)
    {
        printf("Close File Fail...\n");exit(1);
    }
    return students;
}

void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
    int row = 0,inrow=1,val = 0,i=0;
    if(factor > 20 || factor < -20) return; // if incorrect factor
    while(*(students+row) != NULL)
    {
        // start with one because we dont want to count the name
        i=1;
        inrow = 1;
        while(i < coursesPerStudent[row])
        {
            if(strcmp(students[row][inrow],courseName) == 0)
            {
                val = atoi(students[row][inrow+1]);
                val = val + factor;
                // if the val is under 0 or upper 100, we fix it
                if(val >= 100 || val <= 0)
                {
                    if(val >= 100) val = 100;
                    else val = 0;
                }
                itoa(val,students[row][inrow+1]);
                break;
            }
            inrow++;
            i++;
        }
        row++;
    }
    return;
}
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}
/* reverse:  reverse string s in place */
 void reverse(char s[])
 {
     int i, j;
     char c;

     for (i = 0, j = (unsigned)strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
}
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents)
{
    for (int i = 0; i < numberOfStudents; i++)
    {
        printf("name: %s\n*********\n", students[i][0]);
        for (int j = 1; j <= 2 * coursesPerStudent[i]; j += 2)
        {
            printf("course: %s\n", students[i][j]);
            printf("grade: %s\n", students[i][j + 1]);
        }
        printf("\n");
    }
}

void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
    FILE *studentListnew;
    studentListnew = fopen("//Users//orenhayoun//Desktop//שנה א׳//סמסטר ב׳//תכנות מתקדמים c//מטלות/מטלה 4//newstudentList.txt","w");
    if(studentListnew == NULL){
        printf("Unable to open file\n");exit(1);
    }
    int row=0,inrow=0;
    char oneline[MAXLINE] = {"\0"};
    char *token = NULL;
    while(row < numberOfStudents)
    {
        inrow = 0;
        while(inrow < (coursesPerStudent[row] * 2 + 1) )
        {
            strcpy(oneline,students[row][inrow]);
            if(inrow == (coursesPerStudent[row] * 2) )
            {
                token = strtok(oneline,"\r\n"); // in macbook new line is \r\n
                fputs(token,studentListnew);
                fputc('\n', studentListnew);
                break;
            }
            fputs(oneline,studentListnew);
            if(inrow % 2 != 0)
            {
                fputc(',', studentListnew);
                inrow++;
                
            }
            else
            {
                fputc('|', studentListnew);
                inrow++;
            }
        }
        fputc('\n', studentListnew);
        row++;
    }
    fflush(studentListnew);
    if(fclose(studentListnew)!= 0)
    {
        printf("Close File Fail...\n");exit(1);
    }
    // free all arrays in the main
}
void freestudetns(char*** students, int* coursesPerStudent, int numberOfStudents)
{
    int row = 0,inrow = 0;
    while(row < numberOfStudents)
    {
        inrow=0;
        while(inrow < coursesPerStudent[row] * 2 + 1)
        {
            free(students[row][inrow]);
            inrow++;
        }
        free(students[row]);
        row++;
    }
    free(students);
}
void freestruct(Student* stud, int* coursesPerStudent, int numberOfStudents)
{
    int i = 0;
    while(i < numberOfStudents)
    {
        free(stud[i].grades);
        i++;
    }
    free(stud);
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
    FILE* binfile;
    binfile = fopen(fileName,"wb");
    if(!binfile)
    {
        printf("Unable to open file!");
        exit(0);
    }
    fseek(binfile,0,SEEK_SET);
    int i = 0,j=0; // index
    char str[35];
    StudentCourseGrade *temp = NULL; // pointer to struct StudentCourseGrade
    int num = numberOfStudents;
    // fwrite(adress to put the "thing"(string,int,float...) in the file,number of bytes,amount of this,FILE) -
    fwrite(&num,sizeof(int),1, binfile); // the number of students
    while(i < numberOfStudents)
    {
        temp = students[i].grades;
        num = students[i].numberOfCourses;
        strcpy(str, students[i].name);
        fwrite(str,NAME_SIZE,1, binfile); // the name of the student
        fwrite(&num,sizeof(int),1, binfile); // number of courses
        fwrite(temp,NAME_SIZE+sizeof(int),students[i].numberOfCourses, binfile);
        i++;
    }
    fflush(binfile);
    if(fclose(binfile)!=0)
    {
        printf("Close File Fail...");exit(1);
    }
}

Student* readFromBinFile(const char* fileName)
{
    FILE* binfile;
    binfile = fopen(fileName,"rb");
    if(!binfile)
    {
        printf("Unable to open file!");
        exit(0);
    }
    int i = 0,j=0;
    fseek(binfile,0,SEEK_SET);
    int nos = 0; // number of students
    int cps = 0; // coures per student
    char str[35];
    StudentCourseGrade *temp = NULL; // pointer to struct StudentCourseGrade
    fread(&nos, sizeof(int), 1, binfile);
    Student* studlist = (Student*) malloc(nos*sizeof(Student));
    if(studlist == NULL)
    {
        printf("readFromBinFile,studlist : allocation faild...\n");exit(1);
    }
    while(i < nos)
    {
        fread(studlist[i].name,NAME_SIZE, 1, binfile);
        fread(&cps, sizeof(int), 1, binfile);
        studlist[i].numberOfCourses = cps;
        studlist[i].grades = (StudentCourseGrade*) malloc(cps*sizeof(StudentCourseGrade));
        if(studlist[i].grades == NULL)
        {
            printf("readFromBinFile,tudlist[i].grades : allocation faild...\n");exit(1);
        }
        temp = studlist[i].grades;
        fread(temp, NAME_SIZE+sizeof(int),cps , binfile);
        i++;
    }
    if(fclose(binfile)!= 0)
    {
        printf("Close File Fail...\n");exit(1);
    }
    return studlist;
}

Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
    Student* studlist = (Student*) malloc(numberOfStudents * sizeof(Student));
    if(studlist == NULL)
    {
        printf("transformStudentArray,studlist : allocation faild...\n");exit(1);
    }
    StudentCourseGrade* temp = NULL;
    int i = 0,j=0;
    while(i < numberOfStudents)
    {
        strcpy((studlist)[i].name,students[i][j]);
        studlist[i].numberOfCourses = coursesPerStudent[i];
        studlist[i].grades = (StudentCourseGrade*) malloc(coursesPerStudent[i] *sizeof(StudentCourseGrade));
        if(studlist[i].grades == NULL)
        {
            printf("transformStudentArray,studlist[i].grades : allocation faild...\n");exit(1);
        }
        temp = studlist[i].grades;
        while(j < coursesPerStudent[i])
        {
            strcpy(temp->courseName,students[i][j*2+1]);
            temp->grade = atoi(students[i][j*2+2]);
            temp++;
            j++;
        }
        j=0;
        i++;
    }
    return studlist;
}

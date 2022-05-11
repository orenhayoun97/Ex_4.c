#define _CRT_SECURE_NO_WARNINGS
#define _CRTDBG_MAP_ALLOC
// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINE 1023

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
void itoa(int n, char s[]);
void reverse(char s[]);

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
    factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Linear Algebra", +5);
//    printStudentArray(students, coursesPerStudent, numberOfStudents);
    studentsToFile(students, coursesPerStudent, numberOfStudents); //this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)
    
    //Part B
//    Student* transformedStudents,,,, = transformStudentArray(students, coursesPerStudent, numberOfStudents);
//    writeToBinFile("students.bin", transformedStudents, numberOfStudents);
//    Student* testReadStudents = readFromBinFile("students.bin");

    //add code to free all arrays of struct Student


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
    while(fgets(oneline,MAXLINE, studentList)!=NULL){
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
    fclose(studentList);
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
        token = strtok(oneline,"|");
        while(token != NULL)
        {
            students[row][inrow] = (char*) malloc(sizeof(char));
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
            if(inrow % 2 != 0)
            {
                token = strtok(NULL,"|");
            }
            else token = strtok(NULL,",");
            inrow++;
        }
        row++;
    }
    fclose(studentList);
    return students;
}

void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
    int row = 0,inrow=1,val = 0,i=0;
    if(factor > 20 || factor < -20) return;
    while(*(students+row) != NULL)
    {
        i=1;
        inrow = 1;
        while(i < coursesPerStudent[row])
        {
            if(strcmp(students[row][inrow],courseName) == 0)
            {
                val = atoi(students[row][inrow+1]);
                val = val + factor;
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

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
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
    studentListnew = fopen("/Users/orenhayoun/Desktop/שנה א׳/סמסטר ב׳/תכנות מתקדמים c/מטלות/מטלה 4/newstudentList.txt","w");
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
                token = strtok(oneline,"\r\n");
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
    fclose(studentListnew);
    // free all
    row=0;
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
    return;
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
    //add code here
}

//Student* readFromBinFile(const char* fileName)
//{
//    //add code here
//}
//
//Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
//{
//    //add code here
//}

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

// ======================
// 구조체 정의
// ======================

// User 구조체
typedef struct
{
    char id[20];         // 아이디
    char pw[20];         // 패스워드
    char name[20];       // 이름
    char department[20]; // 부서
    char position[20];   // 직급
    char phone[20];      // 전화번호
    char hire_date[15];  // 입사 날짜
    int salary;          // 봉급
} User;

// Evaluation(인사평가) 구조체
typedef struct
{
    char evaluator_id[20]; // 평가자 ID 
    char employee_id[20];  // 피평가자 ID 
    int teamwork_score;    // 팀워크 점수(0~100)
    int attitude_score;    // 태도 점수(0~100)
    char comments[100];    // 코멘트
} Evaluation;

// Attendance (근태) 구조체
typedef struct {
    char user_id[20];
    char date[15];       // YYYY-MM-DD
    char check_in[10];   // 출근 시간 HH:MM
    char check_out[10];  // 퇴근 시간 HH:MM
    float work_hours;    // 근무 시간
    char status[10];     // 근태 상태 (예: 출근, 지각, 결근)
} Attendance;

// Notice (공지사항) 구조체
typedef struct {
    int id;               // 게시글 번호
    char title[50];       // 제목
    char content[500];    // 내용
    char writer_id[20];   // 작성자 ID
    char date[15];        // 작성 날짜 YYYY-MM-DD
} Notice;

// ======================
// 근태 관리 함수 (TUI용)
// ======================
void check_in(User* user);         // 출근 체크
void check_out(User* user);        // 퇴근 체크
void my_attendance(User* user);    // 내 근태 조회
void all_attendance();             // 전체 근태 조회 (관리자)
void attendance_menu(User* user);  // 근태 관리 메뉴 (TUI)

// ======================
// 공지사항 관련
// ======================
void notice_admin_menu(User* user);        // 관리자 공지사항 메뉴
void notice_add(User* user);               // 공지사항 추가
void notice_list_admin();                  // 공지사항 목록 조회 (관리자)
void notice_edit();                        // 공지사항 수정
void notice_employee_menu();               // 직원 공지사항 메뉴
void notice_list_employee();               // 공지사항 목록 조회 (직원)
void notice_view_detail();                 // 공지사항 상세 조회

// ======================
// UI 관련
// ======================
void init_console();                        // 콘솔 초기화 (커서 숨김, 마우스 활성화)
void gotoxy(int x, int y);                  // 좌표 이동
void set_color(int text, int bg);           // 색상 변경
void draw_box(int x, int y, int w, int h, char* title);  // 박스 그리기
void draw_button(int x, int y, char* text, int is_highlight); // 버튼 그리기
int get_mouse_click(int* x, int* y);       // 마우스 클릭 감지

// ======================
// 로그인 관련
// ======================
void create_dummy_data();                   // 테스트용 데이터 생성
int login_process(User* login_user);        // 로그인 처리

// ======================
// 메뉴
// ======================
void show_main_menu(User* user);            // 메인 메뉴 (관리자/직원 분기)
void show_employee_info_menu(User* user);   // 1. 내 정보 / 직원 조회 메뉴 (TUI)

#endif

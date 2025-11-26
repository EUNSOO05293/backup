#define _CRT_SECURE_NO_WARNINGS
#include "common.h"

// ------------------------------------------------------------------
// 상수 정의 (TUI 좌표)
// ------------------------------------------------------------------
#define BOX_X 5
#define BOX_Y 3
#define BOX_W 70
#define BOX_H 18
#define MENU_X 24

#define BTN_Y_1 6   // 출근 체크
#define BTN_Y_2 8   // 퇴근 체크
#define BTN_Y_3 10  // 내 근태 조회
#define BTN_Y_4 12  // 전체 근태 조회 (관리자)
#define BTN_Y_0 16  // 이전 메뉴로
#define BTN_WIDTH 30

// ------------------------------------------------------------------
// 근태 데이터
// ------------------------------------------------------------------
Attendance attendance_list[500];
int attendance_count = 0;

// ------------------------------------------------------------------
// 날짜/시간 자동 생성
// ------------------------------------------------------------------
void get_today(char* buffer) {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    sprintf(buffer, "%04d-%02d-%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

void get_time_now(char* buffer) {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    sprintf(buffer, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
}

// ------------------------------------------------------------------
// 메시지 표시 함수
// ------------------------------------------------------------------
void show_message(const char* msg, int x, int y, int color, int duration_ms) {
    gotoxy(x, y);
    set_color(color, 0);
    printf("%s", msg);
    set_color(15, 0);
    Sleep(duration_ms);
    gotoxy(x, y);
    printf("%*s", (int)strlen(msg), " "); // 메시지 지우기
}

// ------------------------------------------------------------------
// 파일 입출력
// ------------------------------------------------------------------
void load_attendance() {
    FILE* fp = fopen("Attendance.txt", "r");
    if (!fp) return;

    attendance_count = 0;
    while (fscanf(fp, "%s %s %s %s %f %[^\n]\n",
        attendance_list[attendance_count].user_id,
        attendance_list[attendance_count].date,
        attendance_list[attendance_count].check_in,
        attendance_list[attendance_count].check_out,
        &attendance_list[attendance_count].work_hours,
        attendance_list[attendance_count].status) == 6) {
        attendance_count++;
        if (attendance_count >= 500) break;
    }
    fclose(fp);
}

void save_attendance() {
    FILE* fp = fopen("Attendance.txt", "w");
    if (!fp) return;

    for (int i = 0; i < attendance_count; i++) {
        fprintf(fp, "%s %s %s %s %.2f %s\n",
            attendance_list[i].user_id,
            attendance_list[i].date,
            attendance_list[i].check_in,
            attendance_list[i].check_out,
            attendance_list[i].work_hours,
            attendance_list[i].status);
    }
    fclose(fp);
}

// ------------------------------------------------------------------
// 출근 처리
// ------------------------------------------------------------------
void check_in(User* user) {
    char today[20];
    get_today(today);

    for (int i = 0; i < attendance_count; i++) {
        if (strcmp(attendance_list[i].user_id, user->id) == 0 &&
            strcmp(attendance_list[i].date, today) == 0 &&
            strcmp(attendance_list[i].check_out, "-") == 0) {
            show_message("이미 오늘 출근 체크한 기록이 있습니다!", MENU_X, BOX_Y + BOX_H - 2, 11, 1000);
            return;
        }
    }

    Attendance* a = &attendance_list[attendance_count];
    strcpy(a->user_id, user->id);
    strcpy(a->date, today);
    get_time_now(a->check_in);
    strcpy(a->check_out, "-");
    a->work_hours = 0;
    strcpy(a->status, "출근");

    attendance_count++;
    save_attendance();

    show_message("출근 기록 완료!", MENU_X, BOX_Y + BOX_H - 2, 10, 1000);
}

// ------------------------------------------------------------------
// 퇴근 처리
// ------------------------------------------------------------------
void check_out(User* user) {
    char today[20];
    get_today(today);

    for (int i = 0; i < attendance_count; i++) {
        if (strcmp(attendance_list[i].user_id, user->id) == 0 &&
            strcmp(attendance_list[i].date, today) == 0 &&
            strcmp(attendance_list[i].check_out, "-") == 0) {

            get_time_now(attendance_list[i].check_out);

            int in_h, in_m, in_s, out_h, out_m, out_s;
            sscanf(attendance_list[i].check_in, "%d:%d:%d", &in_h, &in_m, &in_s);
            sscanf(attendance_list[i].check_out, "%d:%d:%d", &out_h, &out_m, &out_s);

            int diff_sec = (out_h * 3600 + out_m * 60 + out_s) - (in_h * 3600 + in_m * 60 + in_s);
            if (diff_sec < 0) diff_sec = 0;

            int wh = diff_sec / 3600;
            int wm = (diff_sec % 3600) / 60;
            int ws = diff_sec % 60;

            attendance_list[i].work_hours = wh + wm / 60.0f;
            sprintf(attendance_list[i].status, "%d시간 %d분 %d초 근무", wh, wm, ws);

            save_attendance();

            show_message("퇴근 기록 완료!", MENU_X, BOX_Y + BOX_H - 2, 10, 1200);
            return;
        }
    }

    show_message("오늘 출근 기록이 없습니다!", MENU_X, BOX_Y + BOX_H - 2, 12, 1000);
}

// ------------------------------------------------------------------
// 내 근태 조회
// ------------------------------------------------------------------
void my_attendance(User* user) {
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    system("cls");
    draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "내 근태 기록");

    int line = BOX_Y + 2;
    for (int i = 0; i < attendance_count; i++) {
        if (strcmp(attendance_list[i].user_id, user->id) == 0) {
            gotoxy(BOX_X + 2, line++);
            set_color(15, 0);
            printf("%s | 출근:%s | 퇴근:%s | %s",
                attendance_list[i].date,
                attendance_list[i].check_in,
                attendance_list[i].check_out,
                attendance_list[i].status);
        }
    }

    gotoxy(BOX_X + 2, BOX_Y + BOX_H - 2);
    set_color(11, 0);
    printf("<< 돌아가기 >>");
    set_color(15, 0);
    _getch();
}

// ------------------------------------------------------------------
// 전체 근태 조회 (관리자)
// ------------------------------------------------------------------
void all_attendance() {
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    system("cls");
    draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "전체 근태 기록");

    int line = BOX_Y + 2;
    for (int i = 0; i < attendance_count; i++) {
        gotoxy(BOX_X + 2, line++);
        set_color(15, 0);
        printf("%s (%s) | 출근:%s | 퇴근:%s | %s",
            attendance_list[i].user_id,
            attendance_list[i].date,
            attendance_list[i].check_in,
            attendance_list[i].check_out,
            attendance_list[i].status);
    }

    gotoxy(BOX_X + 2, BOX_Y + BOX_H - 2);
    set_color(11, 0);
    printf("<< 돌아가기 >>");
    set_color(15, 0);
    _getch();
}

// ------------------------------------------------------------------
// 근태 메뉴 TUI
// ------------------------------------------------------------------
void attendance_menu(User* user) {
    int mx, my;
    int is_admin = (strcmp(user->position, "관리자") == 0);

    load_attendance();

    // 메뉴 UI 한 번만 그림 (깜빡임 방지)
    system("cls");
    draw_box(BOX_X, BOX_Y, BOX_W, BOX_H, "근태 관리");
    draw_button(MENU_X, BTN_Y_1, "1. 출근 체크", 0);
    draw_button(MENU_X, BTN_Y_2, "2. 퇴근 체크", 0);
    draw_button(MENU_X, BTN_Y_3, "3. 내 근태 조회", 0);
    if (is_admin) draw_button(MENU_X, BTN_Y_4, "4. 전체 근태 조회", 0);
    draw_button(MENU_X, BTN_Y_0, "0. 이전 메뉴로", 0);

    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hInput, &mode);
    SetConsoleMode(hInput, (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);

    while (1) {
        if (get_mouse_click(&mx, &my)) {
            if (my == BTN_Y_1 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) check_in(user);
            else if (my == BTN_Y_2 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) check_out(user);
            else if (my == BTN_Y_3 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) my_attendance(user);
            else if (is_admin && my == BTN_Y_4 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) all_attendance();
            else if (my == BTN_Y_0 && mx >= MENU_X && mx <= MENU_X + BTN_WIDTH) return;
        }
    }
}

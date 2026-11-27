#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>  // 用于waitForContinue函数（Windows环境）

#define MAX_COLLEGE_LEN 20  // 学院名称最大长度
#define DATA_FILE "score_management.dat"  // 数据持久化文件

// 统一数据结构体（兼容学生和选手信息）
typedef struct ScoreInfo {
    int id;                 // 编号（选手/学生ID）
    char name[50];          // 姓名
    char college[MAX_COLLEGE_LEN];  // 学院（学生可选填，选手必填）
    float score;            // 成绩
    struct ScoreInfo *next; // 链表节点指针
} ScoreInfo;

// 函数声明
// 链表基础操作
ScoreInfo* createNode();
void addNode(ScoreInfo **head);
void freeAllNodes(ScoreInfo **head);
int getNodeCount(ScoreInfo *head);

// 数据持久化操作
void saveToFile(ScoreInfo *head);
void loadFromFile(ScoreInfo **head);

// 成绩管理操作
void inputScores(ScoreInfo **head);
void viewScores(ScoreInfo *head);
void modifyScores(ScoreInfo *head);
void searchScore(ScoreInfo *head);

// 排名统计操作
void showAllRanking(ScoreInfo *head);
void showByCollegeRanking(ScoreInfo *head);
void showScoreStatistic(ScoreInfo *head);

// 界面与工具函数
void clearScreen();
void waitForContinue();
void mainMenu();
void scoreManagementMenu(ScoreInfo **head);
void statRankingMenu(ScoreInfo *head);

// 主函数
int main() {
    ScoreInfo *head = NULL;  // 链表头指针
    int choice;

    // 程序启动时加载历史数据
    loadFromFile(&head);

    printf("========== 主持人成绩综合管理系统 ==========\n");
    printf("欢迎使用！当前数据条数: %d\n", getNodeCount(head));
    printf("======================================\n");

    do {
        mainMenu();
        printf("请选择功能模块 [0-2]: ");
        scanf("%d", &choice);
        getchar();  // 清除输入缓冲区

        switch (choice) {
            case 1:
                scoreManagementMenu(&head);  // 成绩管理（录入/查看/修改）
                break;
            case 2:
                statRankingMenu(head);       // 排名统计（总排名/学院排名/分数统计）
                break;
            case 0:
                // 退出前保存数据
                saveToFile(head);
                freeAllNodes(&head);
                printf("数据已保存，程序退出，感谢使用！\n");
                break;
            default:
                printf("输入错误，请输入0-2之间的数字！\n");
                waitForContinue();
        }
    } while (choice != 0);

    return 0;
}

// 1. 链表基础操作函数
// 创建新节点
ScoreInfo* createNode() {
    ScoreInfo *newNode = (ScoreInfo*)malloc(sizeof(ScoreInfo));
    if (newNode == NULL) {
        printf("内存分配失败！\n");
        return NULL;
    }

    printf("\n请输入编号: ");
    scanf("%d", &newNode->id);
    getchar();  // 清除换行符

    printf("请输入姓名: ");
    fgets(newNode->name, 50, stdin);
    newNode->name[strcspn(newNode->name, "\n")] = '\0';  // 去除换行符

    printf("请输入学院（无则输入\"无\"）: ");
    fgets(newNode->college, MAX_COLLEGE_LEN, stdin);
    newNode->college[strcspn(newNode->college, "\n")] = '\0';

    printf("请输入成绩: ");
    scanf("%f", &newNode->score);
    getchar();  // 清除输入缓冲区

    newNode->next = NULL;
    return newNode;
}

// 添加节点到链表尾部
void addNode(ScoreInfo **head) {
    ScoreInfo *newNode = createNode();
    if (newNode == NULL) {
        return;
    }

    if (*head == NULL) {
        *head = newNode;  // 链表为空时，新节点作为头节点
    } else {
        ScoreInfo *current = *head;
        while (current->next != NULL) {
            current = current->next;  // 遍历到链表尾部
        }
        current->next = newNode;
    }

    printf("信息添加成功！\n");
}

// 释放所有节点内存
void freeAllNodes(ScoreInfo **head) {
    ScoreInfo *current = *head;
    ScoreInfo *nextNode;

    while (current != NULL) {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }

    *head = NULL;  // 头指针置空
}

// 获取节点总数
int getNodeCount(ScoreInfo *head) {
    int count = 0;
    ScoreInfo *current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

// 2. 数据持久化函数
// 保存数据到文件（二进制存储）
void saveToFile(ScoreInfo *head) {
    int count = getNodeCount(head);
    if (count == 0) {
        printf("没有数据可保存！\n");
        return;
    }

    FILE *file = fopen(DATA_FILE, "wb");
    if (file == NULL) {
        printf("文件打开失败，数据保存失败！\n");
        return;
    }

    ScoreInfo *current = head;
    while (current != NULL) {
        fwrite(current, sizeof(ScoreInfo), 1, file);
        current = current->next;
    }

    fclose(file);
    printf("成功保存 %d 条数据到 %s\n", count, DATA_FILE);
}

// 从文件加载数据
void loadFromFile(ScoreInfo **head) {
    // 先清空现有数据
    freeAllNodes(head);

    FILE *file = fopen(DATA_FILE, "rb");
    if (file == NULL) {
        printf("未找到历史数据文件，将创建新数据！\n");
        return;
    }

    ScoreInfo *previous = NULL;
    ScoreInfo *newNode;
    int count = 0;

    while (1) {
        newNode = (ScoreInfo*)malloc(sizeof(ScoreInfo));
        // 读取失败（文件结束）则退出循环
        if (fread(newNode, sizeof(ScoreInfo), 1, file) != 1) {
            free(newNode);
            break;
        }

        newNode->next = NULL;
        if (*head == NULL) {
            *head = newNode;
        } else {
            previous->next = newNode;
        }
        previous = newNode;
        count++;
    }

    fclose(file);
    printf("成功加载 %d 条历史数据\n", count);
}

// 3. 成绩管理操作函数
// 录入成绩（添加节点）
void inputScores(ScoreInfo **head) {
    clearScreen();
    printf("========== 录入成绩 ==========\n");
    addNode(head);
}

// 查看所有成绩
void viewScores(ScoreInfo *head) {
    clearScreen();
    printf("========== 所有成绩列表 ==========\n");
    if (head == NULL) {
        printf("暂无成绩数据！\n");
        return;
    }

    printf("%-6s %-20s %-15s %-10s\n", "编号", "姓名", "学院", "成绩");
    printf("---------------------------------------------\n");

    ScoreInfo *current = head;
    while (current != NULL) {
        printf("%-6d %-20s %-15s %-10.2f\n",
               current->id, current->name,
               current->college, current->score);
        current = current->next;
    }
    printf("=============================================\n");
    printf("总计: %d 条数据\n", getNodeCount(head));
}

// 修改成绩
void modifyScores(ScoreInfo *head) {
    clearScreen();
    printf("========== 修改成绩 ==========\n");
    if (head == NULL) {
        printf("暂无成绩数据可修改！\n");
        return;
    }

    int targetId;
    printf("请输入要修改的编号: ");
    scanf("%d", &targetId);
    getchar();

    ScoreInfo *current = head;
    while (current != NULL) {
        if (current->id == targetId) {
            printf("\n找到编号 %d 的记录：\n", targetId);
            printf("当前姓名: %s, 学院: %s, 成绩: %.2f\n",
                   current->name, current->college, current->score);

            printf("\n请输入新姓名（不变则按回车）: ");
            char newName[50];
            fgets(newName, 50, stdin);
            newName[strcspn(newName, "\n")] = '\0';
            if (strlen(newName) > 0) {
                strcpy(current->name, newName);
            }

            printf("请输入新学院（不变则按回车）: ");
            char newCollege[MAX_COLLEGE_LEN];
            fgets(newCollege, MAX_COLLEGE_LEN, stdin);
            newCollege[strcspn(newCollege, "\n")] = '\0';
            if (strlen(newCollege) > 0) {
                strcpy(current->college, newCollege);
            }

            printf("请输入新成绩（不变则输入-1）: ");
            float newScore;
            scanf("%f", &newScore);
            getchar();
            if (newScore != -1) {
                current->score = newScore;
            }

            printf("修改成功！\n");
            return;
        }
        current = current->next;
    }

    printf("未找到编号为 %d 的记录！\n", targetId);
}

// 查找成绩（按姓名）
void searchScore(ScoreInfo *head) {
    clearScreen();
    printf("========== 查找成绩 ==========\n");
    if (head == NULL) {
        printf("暂无成绩数据！\n");
        return;
    }

    char searchName[50];
    printf("请输入要查找的姓名: ");
    fgets(searchName, 50, stdin);
    searchName[strcspn(searchName, "\n")] = '\0';

    ScoreInfo *current = head;
    int found = 0;
    printf("\n查找结果：\n");
    printf("%-6s %-20s %-15s %-10s\n", "编号", "姓名", "学院", "成绩");
    printf("---------------------------------------------\n");

    while (current != NULL) {
        if (strcmp(current->name, searchName) == 0) {
            printf("%-6d %-20s %-15s %-10.2f\n",
                   current->id, current->name,
                   current->college, current->score);
            found = 1;
        }
        current = current->next;
    }

    if (!found) {
        printf("未找到姓名为 \"%s\" 的记录！\n", searchName);
    }
}

// 4. 排名统计操作函数
// 总排名（降序）
void showAllRanking(ScoreInfo *head) {
    clearScreen();
    printf("========== 总排名（按成绩降序） ==========\n");
    int count = getNodeCount(head);
    if (count == 0) {
        printf("暂无数据可排名！\n");
        return;
    }

    // 复制链表数据到数组（避免修改原链表）
    ScoreInfo *tempArr = (ScoreInfo*)malloc(count * sizeof(ScoreInfo));
    ScoreInfo *current = head;
    for (int i = 0; i < count; i++) {
        tempArr[i] = *current;
        current = current->next;
    }

    // 冒泡排序（降序）
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (tempArr[j].score > tempArr[i].score) {
                ScoreInfo t = tempArr[i];
                tempArr[i] = tempArr[j];
                tempArr[j] = t;
            }
        }
    }

    // 显示排名
    printf("%-6s %-20s %-15s %-10s\n", "名次", "姓名", "学院", "成绩");
    printf("---------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-6d %-20s %-15s %-10.2f\n",
               i + 1, tempArr[i].name,
               tempArr[i].college, tempArr[i].score);
    }

    free(tempArr);
}

// 按学院排名
void showByCollegeRanking(ScoreInfo *head) {
    clearScreen();
    printf("========== 按学院排名 ==========\n");
    if (head == NULL) {
        printf("暂无数据可排名！\n");
        return;
    }

    char targetCollege[MAX_COLLEGE_LEN];
    printf("请输入要查询的学院: ");
    fgets(targetCollege, MAX_COLLEGE_LEN, stdin);
    targetCollege[strcspn(targetCollege, "\n")] = '\0';

    // 统计该学院的人数并复制数据
    int collegeCount = 0;
    ScoreInfo *current = head;
    while (current != NULL) {
        if (strcmp(current->college, targetCollege) == 0) {
            collegeCount++;
        }
        current = current->next;
    }

    if (collegeCount == 0) {
        printf("未找到 \"%s\" 学院的记录！\n", targetCollege);
        return;
    }

    // 复制该学院数据到数组
    ScoreInfo *collegeArr = (ScoreInfo*)malloc(collegeCount * sizeof(ScoreInfo));
    current = head;
    int idx = 0;
    while (current != NULL) {
        if (strcmp(current->college, targetCollege) == 0) {
            collegeArr[idx++] = *current;
        }
        current = current->next;
    }

    // 冒泡排序（降序）
    for (int i = 0; i < collegeCount - 1; i++) {
        for (int j = i + 1; j < collegeCount; j++) {
            if (collegeArr[j].score > collegeArr[i].score) {
                ScoreInfo t = collegeArr[i];
                collegeArr[i] = collegeArr[j];
                collegeArr[j] = t;
            }
        }
    }

    // 显示学院排名
    printf("\n=== %s 学院排名（共 %d 人） ===\n", targetCollege, collegeCount);
    printf("%-6s %-20s %-10s\n", "名次", "姓名", "成绩");
    printf("-----------------------------\n");
    for (int i = 0; i < collegeCount; i++) {
        printf("%-6d %-20s %-10.2f\n",
               i + 1, collegeArr[i].name, collegeArr[i].score);
    }

    free(collegeArr);
}

// 分数统计（优秀/良好/及格/平均分）
void showScoreStatistic(ScoreInfo *head) {
    clearScreen();
    printf("========== 分数统计 ==========\n");
    int count = getNodeCount(head);
    if (count == 0) {
        printf("暂无数据可统计！\n");
        return;
    }

    int excellent = 0, good = 0, pass = 0;
    float totalScore = 0.0f;
    ScoreInfo *current = head;

    while (current != NULL) {
        totalScore += current->score;
        if (current->score >= 8.5) {
            excellent++;  // 优秀（8.5分及以上）
        } else if (current->score >= 7.0) {
            good++;       // 良好（7.0-8.4分）
        } else {
            pass++;       // 及格（7.0分以下，默认60分制转换为10分制逻辑）
        }
        current = current->next;
    }

    float avgScore = totalScore / count;
    printf("总人数: %d\n", count);
    printf("优秀(8.5+): %d人 (%.1f%%)\n", excellent, (excellent * 100.0) / count);
    printf("良好(7.0-8.4): %d人 (%.1f%%)\n", good, (good * 100.0) / count);
    printf("及格(7.0-): %d人 (%.1f%%)\n", pass, (pass * 100.0) / count);
    printf("平均分: %.2f\n", avgScore);
}

// 5. 界面与工具函数
// 清屏函数（Windows用cls，Linux/Mac用clear）
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// 等待用户按任意键继续
void waitForContinue() {
    printf("\n按任意键继续...\n");
    getch();  // 需包含conio.h
}

// 主菜单
void mainMenu() {
    clearScreen();
    printf("========== 成绩综合管理系统 ==========\n");
    printf("1. 成绩管理（录入/查看/修改/查找）\n");
    printf("2. 排名统计（总排名/学院排名/分数统计）\n");
    printf("0. 退出系统\n");
    printf("======================================\n");
}

// 成绩管理子菜单
void scoreManagementMenu(ScoreInfo **head) {
    int choice;
    do {
        clearScreen();
        printf("========== 成绩管理 ==========\n");
        printf("当前数据条数: %d\n", getNodeCount(*head));
        printf("==============================\n");
        printf("1. 录入成绩\n");
        printf("2. 查看所有成绩\n");
        printf("3. 修改成绩\n");
        printf("4. 查找成绩\n");
        printf("0. 返回主菜单\n");
        printf("==============================\n");
        printf("请选择 [0-4]: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                inputScores(head);
                waitForContinue();
                break;
            case 2:
                viewScores(*head);
                waitForContinue();
                break;
            case 3:
                modifyScores(*head);
                waitForContinue();
                break;
            case 4:
                searchScore(*head);
                waitForContinue();
                break;
            case 0:
                break;
            default:
                printf("输入错误，请输入0-4之间的数字！\n");
                waitForContinue();
        }
    } while (choice != 0);
}

// 排名统计子菜单
void statRankingMenu(ScoreInfo *head) {
    int choice;
    do {
        clearScreen();
        printf("========== 排名统计 ==========\n");
        printf("当前数据条数: %d\n", getNodeCount(head));
        printf("==============================\n");
        printf("1. 总排名\n");
        printf("2. 按学院排名\n");
        printf("3. 分数统计\n");
        printf("0. 返回主菜单\n");
        printf("==============================\n");
        printf("请选择 [0-3]: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                showAllRanking(head);
                waitForContinue();
                break;
            case 2:
                showByCollegeRanking(head);
                waitForContinue();
                break;
            case 3:
                showScoreStatistic(head);
                waitForContinue();
                break;
            case 0:
                break;
            default:
                printf("输入错误，请输入0-3之间的数字！\n");
                waitForContinue();
        }
    } while (choice != 0);
}
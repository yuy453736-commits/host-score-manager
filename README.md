# 主持人成绩综合管理系统

C 语言课程设计项目，基于**单链表**存储选手成绩数据，使用**二进制文件**实现数据持久化，关闭程序后数据不丢失。

## 功能清单

1. 选手成绩录入（编号、姓名、学院、成绩）
2. 全部成绩浏览
3. 按编号修改选手信息
4. 按姓名查询选手
5. 全部选手成绩总排名（冒泡排序，降序）
6. 指定学院内部排名
7. 成绩分段统计（优秀 / 良好 / 及格人数、占比、平均分）
8. 二进制文件保存与读取，程序退出前自动保存数据

## 数据结构

```c
typedef struct ScoreInfo {
    int id;                 // 编号
    char name[50];          // 姓名
    char college[20];       // 学院
    float score;            // 成绩
    struct ScoreInfo *next; // 链表节点指针
} ScoreInfo;
```

## 编译与运行

### Windows（MinGW / Dev-C++ / Visual Studio）

```bash
gcc host_score_manager.cpp -o host_score_manager.exe
./host_score_manager.exe
```

> 说明：`conio.h` 与 `getch()` 仅 Windows 环境可用；在 Linux / macOS 下编译时，需将 `waitForContinue()` 中的 `getch()` 替换为 `getchar()`，或注释掉按任意键等待的逻辑。

## 文件说明

| 文件 | 说明 |
| --- | --- |
| `host_score_manager.cpp` | 系统完整源码 |
| `score_management.dat` | 运行时自动生成的成绩数据文件（二进制，无需手动创建） |

## 运行界面

```
========== 成绩综合管理系统 ==========
1. 成绩管理（录入/查看/修改/查找）
2. 排名统计（总排名/学院排名/分数统计）
0. 退出系统
======================================
```

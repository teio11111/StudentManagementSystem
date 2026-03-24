#include <iostream>
#include <string>
#include <memory>
#include <limits>
#include <mysql/jdbc.h>
#include <locale>
#include <sstream>
#include <iomanip>   //对齐表格数据

using std::cin;
using std::cout;
using std::endl;
using std::string;

void showMenu();
void queryAllStudents(sql::Connection* conn);
void addStudent(sql::Connection* conn);
void updateStudent(sql::Connection* conn);
void deleteStudent(sql::Connection* conn);
void waitForEnter();
int readInt(const string& prompt, int minValue = 0);   //自动处理输错情况，清空缓冲区
string readNonEmptyLine(const string& prompt);         //防止空输入
bool confirmAction(const string& prompt);

int main() {
    setlocale(LC_ALL, "");
    system("chcp 65001");
    system("cls");

    try {
        sql::Driver* driver = sql::mysql::get_driver_instance();

        std::unique_ptr<sql::Connection> conn(
            driver->connect("tcp://127.0.0.1:3306", "root", "Root123456")
        );

        conn->setSchema("student_db");

        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        stmt->execute("SET NAMES utf8mb4");

        cout << "数据库连接成功！" << endl;

        while (true) {
            showMenu();
            int choice = readInt("请输入操作选项（1-5）：", 1);

            if (choice < 1 || choice > 5) {
                cout << "输入无效，请重新输入！" << endl;
                waitForEnter();
                continue;
            }
           

            if (choice < 1 || choice > 5) {
                cout << "输入无效，请重新输入！" << endl;
                waitForEnter();
                continue;
            }

            

            switch (choice) {
            case 1:
                queryAllStudents(conn.get());
                break;
            case 2:
                addStudent(conn.get());
                break;
            case 3:
                updateStudent(conn.get());
                break;
            case 4:
                deleteStudent(conn.get());
                break;
            case 5:
                cout << "程序已退出，按回车键关闭窗口..." << endl;
                waitForEnter();
                return 0;
            default:
                cout << "输入无效，请重新输入！" << endl;
            }
        }
    }
    catch (sql::SQLException& e) {
        cout << "MySQL异常: " << e.what() << endl;
        cout << "错误代码: " << e.getErrorCode() << endl;
        cout << "SQLState: " << e.getSQLState() << endl;
    }
    catch (std::exception& e) {
        cout << "标准异常: " << e.what() << endl;
    }
    catch (...) {
        cout << "发生未知异常！" << endl;
    }

    return 0;
}

void showMenu() {
    cout << "\n========== 学生信息管理系统 ==========" << endl;
    cout << "1. 查询所有学生" << endl;
    cout << "2. 添加学生" << endl;
    cout << "3. 修改学生" << endl;
    cout << "4. 删除学生" << endl;
    cout << "5. 退出系统" << endl;
    cout << "====================================" << endl;
}

void queryAllStudents(sql::Connection* conn) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement("SELECT id, name, age, major, class_name FROM students")
        );
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        cout << "\n当前学生信息如下：" << endl;
        cout << "---------------------------------------------------------------------" << endl;
        cout << std::left
            << std::setw(6) << "ID"
            << std::setw(12) << "姓名"
            << std::setw(8) << "年龄"
            << std::setw(25) << "专业"
            << std::setw(12) << "班级" << endl;
        cout << "---------------------------------------------------------------------" << endl;

        bool hasData = false;
        while (res->next()) {
            hasData = true;
            cout << std::left  //左对齐，设置列宽
                << std::setw(6) << res->getInt("id")    
                << std::setw(12) << res->getString("name")
                << std::setw(8) << res->getInt("age")
                << std::setw(25) << res->getString("major")
                << std::setw(12) << res->getString("class_name") << endl;
        }

        if (!hasData) {
            cout << "当前没有学生数据。" << endl;
        }

        waitForEnter();
    }
    catch (sql::SQLException& e) {
        cout << "查询失败: " << e.what() << endl;
        waitForEnter();
    }
}

void addStudent(sql::Connection* conn) {
    try {
        string name, major, class_name;
        int age;

        name = readNonEmptyLine("请输入姓名：");
        age = readInt("请输入年龄：", 0);
        major = readNonEmptyLine("请输入专业：");
        class_name = readNonEmptyLine("请输入班级：");

        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement(
                "INSERT INTO students(name, age, major, class_name) VALUES (?, ?, ?, ?)"
            )
        );

        pstmt->setString(1, name);
        pstmt->setInt(2, age);
        pstmt->setString(3, major);
        pstmt->setString(4, class_name);

        int rows = pstmt->executeUpdate();

        if (rows > 0) {
            cout << "添加成功！" << endl;
        }
        else {
            cout << "添加失败！" << endl;
        }
        waitForEnter();
    }
    catch (sql::SQLException& e) {
        cout << "添加失败: " << e.what() << endl;
        waitForEnter();
    }
}

void updateStudent(sql::Connection* conn) {
    try {
        int id, age;
        string name, major, class_name;

        id = readInt("请输入要修改的学生ID：", 1);
        name = readNonEmptyLine("请输入新姓名：");
        age = readInt("请输入新年龄：", 0);
        major = readNonEmptyLine("请输入新专业：");
        class_name = readNonEmptyLine("请输入新班级：");

        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement(
                "UPDATE students SET name=?, age=?, major=?, class_name=? WHERE id=?"
            )
        );

        pstmt->setString(1, name);
        pstmt->setInt(2, age);
        pstmt->setString(3, major);
        pstmt->setString(4, class_name);
        pstmt->setInt(5, id);

        int rows = pstmt->executeUpdate();

        if (rows > 0) {
            cout << "修改成功！" << endl;
        }
        else {
            cout << "未找到该学生ID，修改失败！" << endl;
        }
        waitForEnter();
    }
    catch (sql::SQLException& e) {
        cout << "修改失败: " << e.what() << endl;
        waitForEnter();
    }
}

void deleteStudent(sql::Connection* conn) {
    try {
        int id;

        id = readInt("请输入要删除的学生ID：", 1);

        if (!confirmAction("确认要删除该学生吗？(Y/N)：")) {
            cout << "已取消删除操作。" << endl;
            waitForEnter();
            return;
        }

        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement("DELETE FROM students WHERE id=?")
        );

        pstmt->setInt(1, id);

        int rows = pstmt->executeUpdate();

        if (rows > 0) {
            cout << "删除成功！" << endl;
        }
        else {
            cout << "未找到该学生ID，删除失败！" << endl;
        }
        waitForEnter();
    }
    catch (sql::SQLException& e) {
        cout << "删除失败: " << e.what() << endl;
        waitForEnter();
    }
}

void waitForEnter() {
    string dummy;
    cout << "按回车键继续...";
    std::getline(cin, dummy);
}

int readInt(const string& prompt, int minValue) {
    while (true) {
        string line;
        cout << prompt;
        std::getline(cin, line);

        std::stringstream ss(line);
        int value;
        char extra;

        if (!(ss >> value) || (ss >> extra)) {
            cout << "输入无效，请输入整数！" << endl;
            continue;
        }

        if (value < minValue) {
            cout << "输入无效，请输入不小于 " << minValue << " 的数字！" << endl;
            continue;
        }

        return value;
    }
}

string readNonEmptyLine(const string& prompt) {
    while (true) {
        string input;
        cout << prompt;
        std::getline(cin, input);

        if (input.empty()) {
            cout << "输入不能为空，请重新输入！" << endl;
            continue;
        }

        return input;
    }
}

bool confirmAction(const string& prompt) {
    while (true) {
        string input;
        cout << prompt;
        std::getline(cin, input);

        if (input == "Y" || input == "y") {
            return true;
        }
        else if (input == "N" || input == "n") {
            return false;
        }
        else {
            cout << "输入无效，请输入 Y 或 N！" << endl;
        }
    }
}
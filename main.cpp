#include <iostream>
#include <string>
#include <memory>
#include <limits>
#include <mysql/jdbc.h>
#include <locale>

using std::cin;
using std::cout;
using std::endl;
using std::string;

void showMenu();
void queryAllStudents(sql::Connection* conn);
void addStudent(sql::Connection* conn);
void updateStudent(sql::Connection* conn);
void deleteStudent(sql::Connection* conn);

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
            int choice = 0;

            showMenu();
            cout << "请输入操作选项（1-5）：";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
                cout << "输入无效，请输入数字！" << endl;
                continue;
            }

            cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

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
                cout << "程序已退出。" << endl;
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

    system("pause");
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
        cout << "-------------------------------------------------------------" << endl;
        cout << "ID\t姓名\t年龄\t专业\t\t班级" << endl;
        cout << "-------------------------------------------------------------" << endl;

        bool hasData = false;
        while (res->next()) {
            hasData = true;
            cout << res->getInt("id") << "\t"
                << res->getString("name") << "\t"
                << res->getInt("age") << "\t"
                << res->getString("major") << "\t"
                << res->getString("class_name") << endl;
        }

        if (!hasData) {
            cout << "当前没有学生数据。" << endl;
        }
    }
    catch (sql::SQLException& e) {
        cout << "查询失败: " << e.what() << endl;
    }
}

void addStudent(sql::Connection* conn) {
    try {
        string name, major, class_name;
        int age;

        cout << "请输入姓名：";
        std::getline(cin, name);

        cout << "请输入年龄：";
        cin >> age;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        cout << "请输入专业：";
        std::getline(cin, major);

        cout << "请输入班级：";
        std::getline(cin, class_name);

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
    }
    catch (sql::SQLException& e) {
        cout << "添加失败: " << e.what() << endl;
    }
}

void updateStudent(sql::Connection* conn) {
    try {
        int id, age;
        string name, major, class_name;

        cout << "请输入要修改的学生ID：";
        cin >> id;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        cout << "请输入新姓名：";
        std::getline(cin, name);

        cout << "请输入新年龄：";
        cin >> age;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        cout << "请输入新专业：";
        std::getline(cin, major);

        cout << "请输入新班级：";
        std::getline(cin, class_name);

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
    }
    catch (sql::SQLException& e) {
        cout << "修改失败: " << e.what() << endl;
    }
}

void deleteStudent(sql::Connection* conn) {
    try {
        int id;

        cout << "请输入要删除的学生ID：";
        cin >> id;
        cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

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
    }
    catch (sql::SQLException& e) {
        cout << "删除失败: " << e.what() << endl;
    }
}
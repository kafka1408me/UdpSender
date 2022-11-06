#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QMessageBox>
#include "window.h"
#include "myapp.h"

#ifdef TEST
#include <iostream>
#endif

const char* semaphoreName = "myUdpSenderSemaphore";

const char* sharedMemoryName = "myUdpSenderMemory";

void checkSingleProcess();

#ifdef TEST
void simple_socket()
{
    try {
        boost::asio::io_service ios;

        boost::asio::ip::udp::socket sock(ios);
        sock.open(boost::asio::ip::udp::v4());

        sock.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 19678));

        char c[1024];
        boost::asio::ip::udp::endpoint from;
        size_t n = sock.receive_from(boost::asio::buffer(c), from);

        std::cout << std::string(c) << std::endl;

        std::string msg = "Hello, UDP!";

        sock.send_to(boost::asio::buffer(msg), from);


    }  catch (boost::system::system_error& e) {

    }
}
#endif

int main(int argc, char** argv)
{
    MyApp app(argc, argv);

    checkSingleProcess();

    Window window;
    window.show();

#ifdef TEST
    std::thread t(simple_socket);
    t.detach();
#endif

    return app.exec();
}

void checkSingleProcess()
{
    // Разрешение запуска только одного экземпляра приложения
    // https://evileg.com/ru/post/147/
    QSystemSemaphore semaphore(semaphoreName, 1);  // создаём семафор
    semaphore.acquire(); // Поднимаем семафор, запрещая другим экземплярам работать с разделяемой памятью

#ifndef Q_OS_WIN32
    // в linux/unix разделяемая память не освобождается при аварийном завершении приложения,
    // поэтому необходимо избавиться от данного мусора
    QSharedMemory nix_fix_shared_memory(sharedMemoryName);
    if(nix_fix_shared_memory.attach()){
        nix_fix_shared_memory.detach();
    }
#endif

    QSharedMemory sharedMemory(sharedMemoryName);  // Создаём экземпляр разделяемой памяти
    bool is_running;            // переменную для проверки ууже запущенного приложения
    if (sharedMemory.attach()){ // пытаемся присоединить экземпляр разделяемой памяти
        // к уже существующему сегменту
        is_running = true;      // Если успешно, то определяем, что уже есть запущенный экземпляр
    }else{
        sharedMemory.create(1); // В противном случае выделяем 1 байт памяти
        is_running = false;     // И определяем, что других экземпляров не запущено
    }
    semaphore.release();        // Опускаем семафор

    // Если уже запущен один экземпляр приложения, то сообщаем об этом пользователю
    // и завершаем работу текущего экземпляра приложения
    if(is_running){
        QMessageBox msgBox;
        msgBox.setStyleSheet("QMessageBox{background-color: #9db3b3} "
                             "QLabel{font: 12pt \"Times New Roman\";} "
                             "QPushButton{"
                             "background-color:#4286b1;"
                             "border: 2px solid #8f8f91;"
                             "border-radius: 10px;"
                             "font: 16px;"
                             "min-width: 100px;"
                             "min-height: 30px;"
                             "}"
                             "QPushButton:pressed {"
                             "background-color:#1673ad ;"
                             "border: 2px solid #c8cf03;"
                             "}"
                             "QPushButton:hover {"
                             "border: 2px solid #dee60c;"
                             "font: bold 18px;"
                             "}");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("The application is already runnig.\n"
                       "You can run only one instance of the application.");
        msgBox.exec();
        exit(-100);
    }
}

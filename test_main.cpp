#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QMainWindow mainWindow;
    QWidget *centralWidget = new QWidget(&mainWindow);
    mainWindow.setCentralWidget(centralWidget);
    
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    QPushButton *button = new QPushButton("Hello World", centralWidget);
    layout->addWidget(button);
    
    mainWindow.setWindowTitle("Test App");
    mainWindow.resize(400, 300);
    mainWindow.show();
    
    return app.exec();
}

#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application information
    QApplication::setApplicationName("LLVM Builder GUI");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("LLVM Tools");
    
    // Create and show the main window
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}

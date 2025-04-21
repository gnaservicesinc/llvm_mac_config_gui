#ifndef BUILDEXECUTOR_H
#define BUILDEXECUTOR_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QTemporaryFile>

class BuilderConfiguration;
class CommandGenerator;

class BuildExecutor : public QObject
{
    Q_OBJECT
    
public:
    explicit BuildExecutor(QObject *parent = nullptr);
    ~BuildExecutor();
    
    // Execute the build command
    void executeBuild(const BuilderConfiguration &config);
    
    // Execute a custom command
    void executeCommand(const QString &command);
    
    // Cancel the current build
    void cancelBuild();
    
    // Check if a build is currently running
    bool isRunning() const;
    
signals:
    // Signal emitted when output is available
    void outputAvailable(const QString &output);
    
    // Signal emitted when the build process finishes
    void buildFinished(bool success, const QString &message);
    
    // Signal emitted when the build process starts
    void buildStarted();
    
private slots:
    // Handle process output
    void handleProcessOutput();
    
    // Handle process error
    void handleProcessError(QProcess::ProcessError error);
    
    // Handle process finished
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    
private:
    QProcess *m_process;
    QTemporaryFile *m_scriptFile;
    
    // Create a temporary script file with the given content
    QString createScriptFile(const QString &content);
};

#endif // BUILDEXECUTOR_H

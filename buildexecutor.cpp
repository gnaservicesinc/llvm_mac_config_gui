#include "buildexecutor.h"
#include "builderconfiguration.h"
#include "commandgenerator.h"

#include <QDir>
#include <QFileInfo>
#include <QTextStream>

BuildExecutor::BuildExecutor(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
    , m_scriptFile(nullptr)
{
    // Connect process signals
    connect(m_process, &QProcess::readyReadStandardOutput, this, &BuildExecutor::handleProcessOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &BuildExecutor::handleProcessOutput);
    connect(m_process, &QProcess::errorOccurred, this, &BuildExecutor::handleProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &BuildExecutor::handleProcessFinished);
}

BuildExecutor::~BuildExecutor()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->terminate();
        m_process->waitForFinished(3000);
        if (m_process->state() != QProcess::NotRunning) {
            m_process->kill();
        }
    }
    
    delete m_scriptFile;
}

void BuildExecutor::executeBuild(const BuilderConfiguration &config)
{
    if (m_process->state() != QProcess::NotRunning) {
        emit outputAvailable("Error: A build process is already running.\n");
        return;
    }
    
    // Generate the build command
    CommandGenerator generator(config);
    QString command = generator.generateBuildCommand();
    
    // Create a temporary script file
    QString scriptPath = createScriptFile(command);
    if (scriptPath.isEmpty()) {
        emit outputAvailable("Error: Failed to create temporary script file.\n");
        emit buildFinished(false, "Failed to create temporary script file");
        return;
    }
    
    // Make the script executable
    QProcess chmodProcess;
    chmodProcess.start("chmod", QStringList() << "+x" << scriptPath);
    if (!chmodProcess.waitForFinished(3000)) {
        emit outputAvailable("Error: Failed to make script executable.\n");
        emit buildFinished(false, "Failed to make script executable");
        return;
    }
    
    // Start the build process
    emit buildStarted();
    emit outputAvailable("Starting build process...\n");
    
    // Set working directory to the build directory
    QDir buildDir(config.buildDir());
    if (!buildDir.exists()) {
        buildDir.mkpath(".");
    }
    m_process->setWorkingDirectory(config.buildDir());
    
    // Start the process
    m_process->start(scriptPath);
}

void BuildExecutor::executeCommand(const QString &command)
{
    if (m_process->state() != QProcess::NotRunning) {
        emit outputAvailable("Error: A process is already running.\n");
        return;
    }
    
    // Create a temporary script file
    QString scriptPath = createScriptFile(command);
    if (scriptPath.isEmpty()) {
        emit outputAvailable("Error: Failed to create temporary script file.\n");
        emit buildFinished(false, "Failed to create temporary script file");
        return;
    }
    
    // Make the script executable
    QProcess chmodProcess;
    chmodProcess.start("chmod", QStringList() << "+x" << scriptPath);
    if (!chmodProcess.waitForFinished(3000)) {
        emit outputAvailable("Error: Failed to make script executable.\n");
        emit buildFinished(false, "Failed to make script executable");
        return;
    }
    
    // Start the process
    emit buildStarted();
    emit outputAvailable("Executing command...\n");
    m_process->start(scriptPath);
}

void BuildExecutor::cancelBuild()
{
    if (m_process->state() == QProcess::NotRunning) {
        return;
    }
    
    emit outputAvailable("Cancelling build process...\n");
    m_process->terminate();
    
    // Wait for the process to terminate
    if (!m_process->waitForFinished(5000)) {
        emit outputAvailable("Process did not terminate gracefully, killing...\n");
        m_process->kill();
    }
}

bool BuildExecutor::isRunning() const
{
    return m_process->state() != QProcess::NotRunning;
}

void BuildExecutor::handleProcessOutput()
{
    // Read standard output
    QByteArray stdoutData = m_process->readAllStandardOutput();
    if (!stdoutData.isEmpty()) {
        emit outputAvailable(QString::fromUtf8(stdoutData));
    }
    
    // Read standard error
    QByteArray stderrData = m_process->readAllStandardError();
    if (!stderrData.isEmpty()) {
        emit outputAvailable(QString::fromUtf8(stderrData));
    }
}

void BuildExecutor::handleProcessError(QProcess::ProcessError error)
{
    QString errorMessage;
    
    switch (error) {
    case QProcess::FailedToStart:
        errorMessage = "The process failed to start.";
        break;
    case QProcess::Crashed:
        errorMessage = "The process crashed.";
        break;
    case QProcess::Timedout:
        errorMessage = "The process timed out.";
        break;
    case QProcess::ReadError:
        errorMessage = "Error reading from the process.";
        break;
    case QProcess::WriteError:
        errorMessage = "Error writing to the process.";
        break;
    case QProcess::UnknownError:
    default:
        errorMessage = "An unknown error occurred.";
        break;
    }
    
    emit outputAvailable("Error: " + errorMessage + "\n");
    emit buildFinished(false, errorMessage);
}

void BuildExecutor::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit outputAvailable("Process completed successfully.\n");
        emit buildFinished(true, "Process completed successfully");
    } else {
        QString message = "Process failed with exit code " + QString::number(exitCode);
        emit outputAvailable(message + "\n");
        emit buildFinished(false, message);
    }
}

QString BuildExecutor::createScriptFile(const QString &content)
{
    // Delete any existing temporary file
    delete m_scriptFile;
    
    // Create a new temporary file
    m_scriptFile = new QTemporaryFile(QDir::tempPath() + "/llvmbuilder_XXXXXX.sh");
    if (!m_scriptFile->open()) {
        return QString();
    }
    
    // Write the content to the file
    QTextStream stream(m_scriptFile);
    stream << "#!/bin/bash\n\n";
    stream << content;
    stream.flush();
    
    // Get the file path and close the file
    QString filePath = m_scriptFile->fileName();
    m_scriptFile->close();
    
    return filePath;
}

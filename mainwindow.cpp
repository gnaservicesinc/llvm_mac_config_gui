#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "builderconfiguration.h"
#include "commandgenerator.h"
#include "buildexecutor.h"
#include "configurationdialog.h"
#include "fielddefaultsmanager.h"

#include <QToolBar>
#include <QLabel>

#include <QFileDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QApplication>
#include <QTextStream>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_config(new BuilderConfiguration())
    , m_generator(new CommandGenerator(*m_config))
    , m_executor(new BuildExecutor(this))
    , m_configDialog(new ConfigurationDialog(this))
{
    ui->setupUi(this);

    // Set up the output text edit
    ui->outputTextEdit->setReadOnly(true);
    ui->outputTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    ui->outputTextEdit->setFont(QFont("Courier New", 10));

    // Connect build executor signals
    connect(m_executor, &BuildExecutor::buildStarted, this, &MainWindow::onBuildStarted);
    connect(m_executor, &BuildExecutor::buildFinished, this, &MainWindow::onBuildFinished);
    connect(m_executor, &BuildExecutor::outputAvailable, this, &MainWindow::onOutputAvailable);

    // Set up the UI
    updateUIFromConfig();
    updateUIState(false);

    // Set up the default buttons
    setupDefaultButtons();

    // Load field defaults
    loadFieldDefaults();

    // Set up the tabs
    ui->tabWidget->setCurrentIndex(0);

    // Set up the status bar
    statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_config;
    delete m_generator;
    delete m_configDialog;
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "About LLVM Builder GUI",
                      "LLVM Builder GUI v1.0\n\n"
                      "A graphical interface for building LLVM with customizable options.\n"
                      "Based on the buildersalone.sh script.");
}

void MainWindow::on_actionSave_Configuration_triggered()
{
    // Update the configuration from the UI
    updateConfigFromUI();

    // Show the configuration dialog
    if (m_configDialog->exec() == QDialog::Accepted) {
        // Get the configuration name
        QString name = m_configDialog->savedConfigurations().isEmpty() ?
                      "Default" : m_configDialog->savedConfigurations().last();

        // Save the configuration
        m_configDialog->saveConfiguration(*m_config, name);

        statusBar()->showMessage("Configuration saved: " + name, 3000);
    }
}

void MainWindow::on_actionLoad_Configuration_triggered()
{
    // Show the configuration dialog
    if (m_configDialog->exec() == QDialog::Accepted) {
        // Get the selected configuration
        QString name = m_configDialog->savedConfigurations().isEmpty() ?
                      "" : m_configDialog->savedConfigurations().last();

        if (!name.isEmpty()) {
            // Load the configuration
            *m_config = m_configDialog->loadConfiguration(name);

            // Update the UI
            updateUIFromConfig();

            statusBar()->showMessage("Configuration loaded: " + name, 3000);
        }
    }
}

void MainWindow::on_actionReset_to_Defaults_triggered()
{
    // Confirm reset
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Reset Configuration",
                                                            "Are you sure you want to reset all settings to defaults?",
                                                            QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    // Reset the configuration
    m_config->resetToDefaults();

    // Update the UI
    updateUIFromConfig();

    statusBar()->showMessage("Configuration reset to defaults", 3000);
}

void MainWindow::on_generateButton_clicked()
{
    // Validate projects and runtimes
    validateProjectsAndRuntimes();

    // Update the configuration from the UI
    updateConfigFromUI();

    // Generate the command
    QString command = m_generator->generateCMakeCommand();

    // Display the command
    ui->commandTextEdit->setPlainText(command);

    // Switch to the output tab
    ui->tabWidget->setCurrentIndex(3);

    statusBar()->showMessage("Command generated", 3000);
}

void MainWindow::on_buildButton_clicked()
{
    // Check if a build is already running
    if (m_executor->isRunning()) {
        QMessageBox::warning(this, "Build in Progress", "A build is already running.");
        return;
    }

    // Validate projects and runtimes
    validateProjectsAndRuntimes();

    // Update the configuration from the UI
    updateConfigFromUI();

    // Confirm if this is not a dry run
    if (!m_config->dryRun()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Start Build",
                                                                "Are you sure you want to start the build?",
                                                                QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            return;
        }
    }

    // Clear the output
    ui->outputTextEdit->clear();

    // Start the build
    m_executor->executeBuild(*m_config);

    // Switch to the output tab
    ui->tabWidget->setCurrentIndex(3);
}

void MainWindow::on_cancelButton_clicked()
{
    // Check if a build is running
    if (!m_executor->isRunning()) {
        return;
    }

    // Confirm cancellation
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Cancel Build",
                                                            "Are you sure you want to cancel the build?",
                                                            QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }

    // Cancel the build
    m_executor->cancelBuild();
}

void MainWindow::on_clearOutputButton_clicked()
{
    ui->outputTextEdit->clear();
}

void MainWindow::on_saveOutputButton_clicked()
{
    // Get the output text
    QString output = ui->outputTextEdit->toPlainText();
    if (output.isEmpty()) {
        QMessageBox::information(this, "Save Output", "There is no output to save.");
        return;
    }

    // Get the file path
    QString fileName = QFileDialog::getSaveFileName(this, "Save Output",
                                                  QDir::homePath() + "/llvm_build_output.txt",
                                                  "Text Files (*.txt)");
    if (fileName.isEmpty()) {
        return;
    }

    // Save the output
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << output;
        file.close();

        statusBar()->showMessage("Output saved to: " + fileName, 3000);
    } else {
        QMessageBox::warning(this, "Save Error", "Failed to save output to: " + fileName);
    }
}

void MainWindow::on_copyCommandButton_clicked()
{
    // Get the command text
    QString command = ui->commandTextEdit->toPlainText();
    if (command.isEmpty()) {
        QMessageBox::information(this, "Copy Command", "There is no command to copy.");
        return;
    }

    // Copy to clipboard
    QApplication::clipboard()->setText(command);

    statusBar()->showMessage("Command copied to clipboard", 3000);
}

void MainWindow::on_botModeCheckBox_toggled(bool checked)
{
    if (checked) {
        // Apply bot mode settings
        applyBotModeSettings();
    }

    // Update UI state
    ui->sudoInstallCheckBox->setEnabled(!checked);
    ui->doNotWarnCheckBox->setEnabled(!checked);
    ui->noLtoCheckBox->setEnabled(!checked);
    ui->doTestingCheckBox->setEnabled(!checked);
    ui->benchmarkCheckBox->setEnabled(!checked);
    ui->terminfoCheckBox->setEnabled(!checked);
    ui->zlibCheckBox->setEnabled(!checked);
    ui->xml2CheckBox->setEnabled(!checked);
    ui->modulesCheckBox->setEnabled(!checked);
    ui->ffiCheckBox->setEnabled(!checked);
}

void MainWindow::on_dryRunCheckBox_toggled(bool checked)
{
    // Update UI state
    ui->buildButton->setText(checked ? "Generate Only" : "Build");
}

void MainWindow::on_doInstallCheckBox_toggled(bool checked)
{
    // Update UI state
    ui->sudoInstallCheckBox->setEnabled(checked);
}

void MainWindow::on_noLtoCheckBox_toggled(bool checked)
{
    // If Disable LTO is checked, uncheck and disable Full LTO
    if (checked) {
        ui->fullLtoCheckBox->setChecked(false);
    }
}

void MainWindow::on_fullLtoCheckBox_toggled(bool checked)
{
    // If Full LTO is checked, uncheck Disable LTO
    if (checked) {
        ui->noLtoCheckBox->setChecked(false);
    }
}

void MainWindow::on_browseCompilerPathButton_clicked()
{
    QString dir = browseForDirectory("Select Compiler Path", ui->compilerPathLineEdit->text());
    if (!dir.isEmpty()) {
        ui->compilerPathLineEdit->setText(dir);
    }
}

void MainWindow::on_browseLlvmDirButton_clicked()
{
    QString dir = browseForDirectory("Select LLVM Directory", ui->llvmDirLineEdit->text());
    if (!dir.isEmpty()) {
        ui->llvmDirLineEdit->setText(dir);
    }
}

void MainWindow::on_browseBuildDirButton_clicked()
{
    QString dir = browseForDirectory("Select Build Directory", ui->buildDirLineEdit->text());
    if (!dir.isEmpty()) {
        ui->buildDirLineEdit->setText(dir);
    }
}

void MainWindow::on_browseInstallPathButton_clicked()
{
    QString dir = browseForDirectory("Select Install Path", ui->installPathLineEdit->text());
    if (!dir.isEmpty()) {
        ui->installPathLineEdit->setText(dir);
    }
}

void MainWindow::on_browseTimerFileButton_clicked()
{
    QString file = browseForFile("Select Timer File", ui->timerFileLineEdit->text(), "Text Files (*.txt)");
    if (!file.isEmpty()) {
        ui->timerFileLineEdit->setText(file);
    }
}

void MainWindow::on_projectsLineEdit_editingFinished()
{
    // Validate when the user finishes editing the projects field
    validateProjectsAndRuntimes();

    // Update checkboxes to match the text
    updateProjectCheckboxes(ui->projectsLineEdit->text());
}

void MainWindow::on_runtimesLineEdit_editingFinished()
{
    // Validate when the user finishes editing the runtimes field
    validateProjectsAndRuntimes();

    // Update checkboxes to match the text
    updateRuntimeCheckboxes(ui->runtimesLineEdit->text());
}

void MainWindow::updateCheckboxesFromText()
{
    // Update both project and runtime checkboxes based on the text fields
    updateProjectCheckboxes(ui->projectsLineEdit->text());
    updateRuntimeCheckboxes(ui->runtimesLineEdit->text());
}

void MainWindow::updateProjectCheckboxes(const QString &projects)
{
    // Split the projects string into a list
    QStringList projectsList = projects.split(";", Qt::SkipEmptyParts);

    // Set the state of each checkbox based on whether its project is in the list
    ui->boltCheckBox->setChecked(projectsList.contains("bolt"));
    ui->clangCheckBox->setChecked(projectsList.contains("clang"));
    ui->clangToolsExtraCheckBox->setChecked(projectsList.contains("clang-tools-extra"));
    ui->compilerRtProjectCheckBox->setChecked(projectsList.contains("compiler-rt"));
    ui->crossProjectTestsCheckBox->setChecked(projectsList.contains("cross-project-tests"));
    ui->libcProjectCheckBox->setChecked(projectsList.contains("libc"));
    ui->libclcCheckBox->setChecked(projectsList.contains("libclc"));
    ui->lldCheckBox->setChecked(projectsList.contains("lld"));
    ui->lldbCheckBox->setChecked(projectsList.contains("lldb"));
    ui->mlirCheckBox->setChecked(projectsList.contains("mlir"));
    ui->openmpProjectCheckBox->setChecked(projectsList.contains("openmp"));
    ui->pollyCheckBox->setChecked(projectsList.contains("polly"));
    ui->pstlProjectCheckBox->setChecked(projectsList.contains("pstl"));
    ui->flangCheckBox->setChecked(projectsList.contains("flang"));
}

void MainWindow::updateRuntimeCheckboxes(const QString &runtimes)
{
    // Split the runtimes string into a list
    QStringList runtimesList = runtimes.split(";", Qt::SkipEmptyParts);

    // Set the state of each checkbox based on whether its runtime is in the list
    ui->libcRuntimeCheckBox->setChecked(runtimesList.contains("libc"));
    ui->libunwindCheckBox->setChecked(runtimesList.contains("libunwind"));
    ui->libcxxabiCheckBox->setChecked(runtimesList.contains("libcxxabi"));
    ui->pstlRuntimeCheckBox->setChecked(runtimesList.contains("pstl"));
    ui->libcxxCheckBox->setChecked(runtimesList.contains("libcxx"));
    ui->compilerRtRuntimeCheckBox->setChecked(runtimesList.contains("compiler-rt"));
    ui->openmpRuntimeCheckBox->setChecked(runtimesList.contains("openmp"));
    ui->llvmLibgccCheckBox->setChecked(runtimesList.contains("llvm-libgcc"));
    ui->offloadCheckBox->setChecked(runtimesList.contains("offload"));
}

void MainWindow::updateTextFromCheckboxes()
{
    // Update the text fields based on the checkboxes
    ui->projectsLineEdit->setText(getProjectsFromCheckboxes());
    ui->runtimesLineEdit->setText(getRuntimesFromCheckboxes());
}

QString MainWindow::getProjectsFromCheckboxes()
{
    QStringList projects;

    // Add each checked project to the list
    if (ui->boltCheckBox->isChecked()) projects.append("bolt");
    if (ui->clangCheckBox->isChecked()) projects.append("clang");
    if (ui->clangToolsExtraCheckBox->isChecked()) projects.append("clang-tools-extra");
    if (ui->compilerRtProjectCheckBox->isChecked()) projects.append("compiler-rt");
    if (ui->crossProjectTestsCheckBox->isChecked()) projects.append("cross-project-tests");
    if (ui->libcProjectCheckBox->isChecked()) projects.append("libc");
    if (ui->libclcCheckBox->isChecked()) projects.append("libclc");
    if (ui->lldCheckBox->isChecked()) projects.append("lld");
    if (ui->lldbCheckBox->isChecked()) projects.append("lldb");
    if (ui->mlirCheckBox->isChecked()) projects.append("mlir");
    if (ui->openmpProjectCheckBox->isChecked()) projects.append("openmp");
    if (ui->pollyCheckBox->isChecked()) projects.append("polly");
    if (ui->pstlProjectCheckBox->isChecked()) projects.append("pstl");
    if (ui->flangCheckBox->isChecked()) projects.append("flang");

    // Join the list into a semicolon-separated string
    return projects.join(";");
}

QString MainWindow::getRuntimesFromCheckboxes()
{
    QStringList runtimes;

    // Add each checked runtime to the list
    if (ui->libcRuntimeCheckBox->isChecked()) runtimes.append("libc");
    if (ui->libunwindCheckBox->isChecked()) runtimes.append("libunwind");
    if (ui->libcxxabiCheckBox->isChecked()) runtimes.append("libcxxabi");
    if (ui->pstlRuntimeCheckBox->isChecked()) runtimes.append("pstl");
    if (ui->libcxxCheckBox->isChecked()) runtimes.append("libcxx");
    if (ui->compilerRtRuntimeCheckBox->isChecked()) runtimes.append("compiler-rt");
    if (ui->openmpRuntimeCheckBox->isChecked()) runtimes.append("openmp");
    if (ui->llvmLibgccCheckBox->isChecked()) runtimes.append("llvm-libgcc");
    if (ui->offloadCheckBox->isChecked()) runtimes.append("offload");

    // Join the list into a semicolon-separated string
    return runtimes.join(";");
}

void MainWindow::handleProjectCheckboxToggled(const QString &project, bool checked)
{
    // If a project checkbox is checked, uncheck the corresponding runtime checkbox if it exists
    if (checked) {
        if (project == "libc") {
            ui->libcRuntimeCheckBox->setChecked(false);
        } else if (project == "compiler-rt") {
            ui->compilerRtRuntimeCheckBox->setChecked(false);
        } else if (project == "openmp") {
            ui->openmpRuntimeCheckBox->setChecked(false);
        } else if (project == "pstl") {
            ui->pstlRuntimeCheckBox->setChecked(false);
        }
    }

    // Update the text field
    updateTextFromCheckboxes();
}

void MainWindow::handleRuntimeCheckboxToggled(const QString &runtime, bool checked)
{
    // If a runtime checkbox is checked, uncheck the corresponding project checkbox if it exists
    if (checked) {
        if (runtime == "libc") {
            ui->libcProjectCheckBox->setChecked(false);
        } else if (runtime == "compiler-rt") {
            ui->compilerRtProjectCheckBox->setChecked(false);
        } else if (runtime == "openmp") {
            ui->openmpProjectCheckBox->setChecked(false);
        } else if (runtime == "pstl") {
            ui->pstlProjectCheckBox->setChecked(false);
        }
    }

    // Update the text field
    updateTextFromCheckboxes();
}

void MainWindow::onBuildStarted()
{
    // Update UI state
    updateUIState(true);

    // Update status bar
    statusBar()->showMessage("Build started");
}

void MainWindow::onBuildFinished(bool success, const QString &message)
{
    // Update UI state
    updateUIState(false);

    // Update status bar
    statusBar()->showMessage(success ? "Build completed successfully" : "Build failed: " + message);
}

void MainWindow::onOutputAvailable(const QString &output)
{
    // Append the output
    ui->outputTextEdit->appendPlainText(output);

    // Scroll to the bottom
    ui->outputTextEdit->verticalScrollBar()->setValue(ui->outputTextEdit->verticalScrollBar()->maximum());
}

void MainWindow::updateUIFromConfig()
{
    // Update path fields
    ui->compilerPathLineEdit->setText(m_config->compilerPath());
    ui->llvmDirLineEdit->setText(m_config->llvmDir());
    ui->buildDirLineEdit->setText(m_config->buildDir());
    ui->installPathLineEdit->setText(m_config->installPath());
    ui->timerFileLineEdit->setText(m_config->timerFile());

    // Update compiler fields
    ui->projectsLineEdit->setText(m_config->projects());
    ui->runtimesLineEdit->setText(m_config->runtimes());

    // Update project and runtime checkboxes
    updateCheckboxesFromText();

    ui->cleanBuildDirCheckBox->setChecked(m_config->cleanBuildDir());
    ui->compilerLineEdit->setText(m_config->compiler());
    ui->cxxCompilerLineEdit->setText(m_config->cxxCompiler());
    ui->linkerLineEdit->setText(m_config->linker());

    // Set the optimization level combo box
    int optIndex = ui->optLevelComboBox->findText(m_config->optLevel());
    if (optIndex >= 0) {
        ui->optLevelComboBox->setCurrentIndex(optIndex);
    }

    // Set the architecture combo boxes
    int archIndex = ui->archComboBox->findText(m_config->arch());
    if (archIndex >= 0) {
        ui->archComboBox->setCurrentIndex(archIndex);
    }

    int osxArchIndex = ui->osxArchComboBox->findText(m_config->osxArch());
    if (osxArchIndex >= 0) {
        ui->osxArchComboBox->setCurrentIndex(osxArchIndex);
    }

    // Update build options
    ui->dryRunCheckBox->setChecked(m_config->dryRun());
    ui->ffiCheckBox->setChecked(m_config->ffi());
    ui->zlibCheckBox->setChecked(m_config->zlib());
    ui->terminfoCheckBox->setChecked(m_config->terminfo());
    ui->xml2CheckBox->setChecked(m_config->xml2());
    ui->noLtoCheckBox->setChecked(m_config->noLto());
    ui->fullLtoCheckBox->setChecked(m_config->fullLto());
    ui->useMakeCheckBox->setChecked(m_config->useMake());
    ui->skipGitPullCheckBox->setChecked(m_config->skipGitPull());
    ui->doInstallCheckBox->setChecked(m_config->doInstall());
    ui->sudoInstallCheckBox->setChecked(m_config->sudoInstall());
    ui->useLocalPythonCheckBox->setChecked(m_config->useLocalPython());
    ui->useDylibCheckBox->setChecked(m_config->useDylib());
    ui->xcodeToolchainCheckBox->setChecked(m_config->xcodeToolchain());
    ui->useXcodeGccCheckBox->setChecked(m_config->useXcodeGcc());
    ui->modulesCheckBox->setChecked(m_config->modules());
    ui->backtracesCheckBox->setChecked(m_config->backtraces());
    ui->doNotWarnCheckBox->setChecked(m_config->doNotWarn());
    ui->doTestingCheckBox->setChecked(m_config->doTesting());
    ui->benchmarkCheckBox->setChecked(m_config->benchmark());
    ui->botModeCheckBox->setChecked(m_config->botMode());

    // Update dependent UI states
    ui->sudoInstallCheckBox->setEnabled(m_config->doInstall());
    ui->buildButton->setText(m_config->dryRun() ? "Generate Only" : "Build");

    // Ensure LTO checkboxes are properly synchronized
    if (m_config->noLto() && m_config->fullLto()) {
        // If both are set (which shouldn't happen), prioritize noLto
        ui->noLtoCheckBox->setChecked(true);
        ui->fullLtoCheckBox->setChecked(false);
    }

    // Update bot mode UI state
    if (m_config->botMode()) {
        applyBotModeSettings();
    }
}

void MainWindow::updateConfigFromUI()
{
    // Update path settings
    m_config->setCompilerPath(ui->compilerPathLineEdit->text());
    m_config->setLlvmDir(ui->llvmDirLineEdit->text());
    m_config->setBuildDir(ui->buildDirLineEdit->text());
    m_config->setInstallPath(ui->installPathLineEdit->text());
    m_config->setTimerFile(ui->timerFileLineEdit->text());

    // Update text fields from checkboxes
    updateTextFromCheckboxes();

    // Update compiler settings
    m_config->setProjects(ui->projectsLineEdit->text());
    m_config->setRuntimes(ui->runtimesLineEdit->text());
    m_config->setCleanBuildDir(ui->cleanBuildDirCheckBox->isChecked());
    m_config->setCompiler(ui->compilerLineEdit->text());
    m_config->setCxxCompiler(ui->cxxCompilerLineEdit->text());
    m_config->setLinker(ui->linkerLineEdit->text());
    m_config->setOptLevel(ui->optLevelComboBox->currentText());
    m_config->setArch(ui->archComboBox->currentText());
    m_config->setOsxArch(ui->osxArchComboBox->currentText());

    // Update build options
    m_config->setDryRun(ui->dryRunCheckBox->isChecked());
    m_config->setFfi(ui->ffiCheckBox->isChecked());
    m_config->setZlib(ui->zlibCheckBox->isChecked());
    m_config->setTerminfo(ui->terminfoCheckBox->isChecked());
    m_config->setXml2(ui->xml2CheckBox->isChecked());
    m_config->setNoLto(ui->noLtoCheckBox->isChecked());
    m_config->setFullLto(ui->fullLtoCheckBox->isChecked());
    m_config->setUseMake(ui->useMakeCheckBox->isChecked());
    m_config->setSkipGitPull(ui->skipGitPullCheckBox->isChecked());
    m_config->setDoInstall(ui->doInstallCheckBox->isChecked());
    m_config->setSudoInstall(ui->sudoInstallCheckBox->isChecked());
    m_config->setUseLocalPython(ui->useLocalPythonCheckBox->isChecked());
    m_config->setUseDylib(ui->useDylibCheckBox->isChecked());
    m_config->setXcodeToolchain(ui->xcodeToolchainCheckBox->isChecked());
    m_config->setUseXcodeGcc(ui->useXcodeGccCheckBox->isChecked());
    m_config->setModules(ui->modulesCheckBox->isChecked());
    m_config->setBacktraces(ui->backtracesCheckBox->isChecked());
    m_config->setDoNotWarn(ui->doNotWarnCheckBox->isChecked());
    m_config->setDoTesting(ui->doTestingCheckBox->isChecked());
    m_config->setBenchmark(ui->benchmarkCheckBox->isChecked());
    m_config->setBotMode(ui->botModeCheckBox->isChecked());

    // Update the command generator
    delete m_generator;
    m_generator = new CommandGenerator(*m_config);
}

void MainWindow::updateUIState(bool buildRunning)
{
    // Enable/disable UI elements based on build state
    ui->buildButton->setEnabled(!buildRunning);
    ui->generateButton->setEnabled(!buildRunning);
    ui->cancelButton->setEnabled(buildRunning);

    // Enable/disable configuration tabs
    ui->tabWidget->setTabEnabled(0, !buildRunning);
    ui->tabWidget->setTabEnabled(1, !buildRunning);
    ui->tabWidget->setTabEnabled(2, !buildRunning);

    // Enable/disable menu actions
    ui->actionSave_Configuration->setEnabled(!buildRunning);
    ui->actionLoad_Configuration->setEnabled(!buildRunning);
    ui->actionReset_to_Defaults->setEnabled(!buildRunning);
}

void MainWindow::applyBotModeSettings()
{
    // Apply bot mode settings as per the script
    ui->sudoInstallCheckBox->setChecked(false);
    ui->doNotWarnCheckBox->setChecked(true);
    ui->noLtoCheckBox->setChecked(true);
    ui->doTestingCheckBox->setChecked(false);
    ui->benchmarkCheckBox->setChecked(false);
    ui->terminfoCheckBox->setChecked(false);
    ui->doInstallCheckBox->setChecked(true);
    ui->zlibCheckBox->setChecked(false);
    ui->xml2CheckBox->setChecked(false);
    ui->modulesCheckBox->setChecked(false);
    ui->ffiCheckBox->setChecked(false);
}

QString MainWindow::browseForDirectory(const QString &title, const QString &startDir)
{
    return QFileDialog::getExistingDirectory(this, title, startDir, QFileDialog::ShowDirsOnly);
}

QString MainWindow::browseForFile(const QString &title, const QString &startDir, const QString &filter)
{
    return QFileDialog::getSaveFileName(this, title, startDir, filter);
}

// Project checkbox handlers
void MainWindow::on_boltCheckBox_toggled(bool checked)
{
    handleProjectCheckboxToggled("bolt", checked);
}

void MainWindow::on_clangCheckBox_toggled(bool checked)
{
    handleProjectCheckboxToggled("clang", checked);
}

void MainWindow::on_clangToolsExtraCheckBox_toggled(bool checked)
{
    handleProjectCheckboxToggled("clang-tools-extra", checked);
}

void MainWindow::on_compilerRtProjectCheckBox_toggled(bool checked)
{
    handleProjectCheckboxToggled("compiler-rt", checked);
}

void MainWindow::on_crossProjectTestsCheckBox_toggled(bool checked)
{
    handleProjectCheckboxToggled("cross-project-tests", checked);
}

void MainWindow::on_libcProjectCheckBox_toggled(bool checked)
{
    handleProjectCheckboxToggled("libc", checked);
}

void MainWindow::on_libclcCheckBox_toggled(bool checked)
{
    handleProjectCheckboxToggled("libclc", checked);
}

void MainWindow::on_lldCheckBox_toggled(bool checked)
{
    handleProjectCheckboxToggled("lld", checked);
}

void MainWindow::on_lldbCheckBox_toggled(bool checked)
{
    handleProjectCheckboxToggled("lldb", checked);
}

void MainWindow::on_mlirCheckBox_toggled(bool checked)
{
    handleProjectCheckboxToggled("mlir", checked);
}

void MainWindow::on_openmpProjectCheckBox_toggled(bool checked)
{
    handleProjectCheckboxToggled("openmp", checked);
}

void MainWindow::on_pollyCheckBox_toggled(bool checked)
{
    handleProjectCheckboxToggled("polly", checked);
}

void MainWindow::on_pstlProjectCheckBox_toggled(bool checked)
{
    handleProjectCheckboxToggled("pstl", checked);
}

void MainWindow::on_flangCheckBox_toggled(bool checked)
{
    handleProjectCheckboxToggled("flang", checked);
}

// Runtime checkbox handlers
void MainWindow::on_libcRuntimeCheckBox_toggled(bool checked)
{
    handleRuntimeCheckboxToggled("libc", checked);
}

void MainWindow::on_libunwindCheckBox_toggled(bool checked)
{
    handleRuntimeCheckboxToggled("libunwind", checked);
}

void MainWindow::on_libcxxabiCheckBox_toggled(bool checked)
{
    handleRuntimeCheckboxToggled("libcxxabi", checked);
}

void MainWindow::on_pstlRuntimeCheckBox_toggled(bool checked)
{
    handleRuntimeCheckboxToggled("pstl", checked);
}

void MainWindow::on_libcxxCheckBox_toggled(bool checked)
{
    handleRuntimeCheckboxToggled("libcxx", checked);
}

void MainWindow::on_compilerRtRuntimeCheckBox_toggled(bool checked)
{
    handleRuntimeCheckboxToggled("compiler-rt", checked);
}

void MainWindow::on_openmpRuntimeCheckBox_toggled(bool checked)
{
    handleRuntimeCheckboxToggled("openmp", checked);
}

void MainWindow::on_llvmLibgccCheckBox_toggled(bool checked)
{
    handleRuntimeCheckboxToggled("llvm-libgcc", checked);
}

void MainWindow::on_offloadCheckBox_toggled(bool checked)
{
    handleRuntimeCheckboxToggled("offload", checked);
}

QStringList MainWindow::findDuplicates(const QString &projects, const QString &runtimes)
{
    QStringList duplicates;

    // Split the projects and runtimes strings into lists
    QStringList projectsList = projects.split(";", Qt::SkipEmptyParts);
    QStringList runtimesList = runtimes.split(";", Qt::SkipEmptyParts);

    // Check for duplicates
    for (const QString &project : projectsList) {
        if (runtimesList.contains(project)) {
            duplicates.append(project);
        }
    }

    return duplicates;
}

void MainWindow::validateProjectsAndRuntimes()
{
    // Get the current projects and runtimes
    QString projects = ui->projectsLineEdit->text();
    QString runtimes = ui->runtimesLineEdit->text();

    // Find duplicates
    QStringList duplicates = findDuplicates(projects, runtimes);

    // If there are duplicates, automatically resolve them
    if (!duplicates.isEmpty()) {
        // Split the projects and runtimes strings into lists
        QStringList projectsList = projects.split(";", Qt::SkipEmptyParts);
        QStringList runtimesList = runtimes.split(";", Qt::SkipEmptyParts);

        // Remove duplicates from projects list (following LLVM guidance)
        for (const QString &duplicate : duplicates) {
            projectsList.removeAll(duplicate);
        }

        // Join the lists back into strings
        QString newProjects = projectsList.join(";");

        // Update the UI
        ui->projectsLineEdit->setText(newProjects);

        // Inform the user about the automatic resolution
        QString duplicatesList = duplicates.join(", ");
        QMessageBox::information(this, "Duplicates Resolved",
                              "The following items appeared in both Projects and Runtimes lists:\n\n" +
                              duplicatesList + "\n\n" +
                              "Following LLVM guidance, these items have been kept in the Runtimes list and removed from the Projects list.");
    }
}

void MainWindow::setupDefaultButtons()
{
    // Create a toolbar for the "Set as Default" buttons
    QToolBar *defaultsToolbar = new QToolBar("Default Values", this);
    defaultsToolbar->setObjectName("defaultsToolbar");
    defaultsToolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBar(Qt::TopToolBarArea, defaultsToolbar);

    // Add a label to explain the toolbar
    QLabel *toolbarLabel = new QLabel("Set Current Field Value as Default: ", this);
    defaultsToolbar->addWidget(toolbarLabel);

    // Create buttons for each field
    QPushButton *compilerPathDefaultButton = new QPushButton("Compiler Path", this);
    compilerPathDefaultButton->setObjectName("compilerPathDefaultButton");
    defaultsToolbar->addWidget(compilerPathDefaultButton);

    QPushButton *llvmDirDefaultButton = new QPushButton("LLVM Directory", this);
    llvmDirDefaultButton->setObjectName("llvmDirDefaultButton");
    defaultsToolbar->addWidget(llvmDirDefaultButton);

    QPushButton *buildDirDefaultButton = new QPushButton("Build Directory", this);
    buildDirDefaultButton->setObjectName("buildDirDefaultButton");
    defaultsToolbar->addWidget(buildDirDefaultButton);

    QPushButton *installPathDefaultButton = new QPushButton("Install Path", this);
    installPathDefaultButton->setObjectName("installPathDefaultButton");
    defaultsToolbar->addWidget(installPathDefaultButton);

    QPushButton *timerFileDefaultButton = new QPushButton("Timer File", this);
    timerFileDefaultButton->setObjectName("timerFileDefaultButton");
    defaultsToolbar->addWidget(timerFileDefaultButton);

    defaultsToolbar->addSeparator();

    QPushButton *projectsDefaultButton = new QPushButton("Projects", this);
    projectsDefaultButton->setObjectName("projectsDefaultButton");
    defaultsToolbar->addWidget(projectsDefaultButton);

    QPushButton *runtimesDefaultButton = new QPushButton("Runtimes", this);
    runtimesDefaultButton->setObjectName("runtimesDefaultButton");
    defaultsToolbar->addWidget(runtimesDefaultButton);

    defaultsToolbar->addSeparator();

    QPushButton *compilerDefaultButton = new QPushButton("Compiler", this);
    compilerDefaultButton->setObjectName("compilerDefaultButton");
    defaultsToolbar->addWidget(compilerDefaultButton);

    QPushButton *cxxCompilerDefaultButton = new QPushButton("C++ Compiler", this);
    cxxCompilerDefaultButton->setObjectName("cxxCompilerDefaultButton");
    defaultsToolbar->addWidget(cxxCompilerDefaultButton);

    QPushButton *linkerDefaultButton = new QPushButton("Linker", this);
    linkerDefaultButton->setObjectName("linkerDefaultButton");
    defaultsToolbar->addWidget(linkerDefaultButton);

    // Connect the buttons to the slot
    connect(compilerPathDefaultButton, &QPushButton::clicked, this, &MainWindow::onSetDefaultButtonClicked);
    connect(llvmDirDefaultButton, &QPushButton::clicked, this, &MainWindow::onSetDefaultButtonClicked);
    connect(buildDirDefaultButton, &QPushButton::clicked, this, &MainWindow::onSetDefaultButtonClicked);
    connect(installPathDefaultButton, &QPushButton::clicked, this, &MainWindow::onSetDefaultButtonClicked);
    connect(timerFileDefaultButton, &QPushButton::clicked, this, &MainWindow::onSetDefaultButtonClicked);
    connect(projectsDefaultButton, &QPushButton::clicked, this, &MainWindow::onSetDefaultButtonClicked);
    connect(runtimesDefaultButton, &QPushButton::clicked, this, &MainWindow::onSetDefaultButtonClicked);
    connect(compilerDefaultButton, &QPushButton::clicked, this, &MainWindow::onSetDefaultButtonClicked);
    connect(cxxCompilerDefaultButton, &QPushButton::clicked, this, &MainWindow::onSetDefaultButtonClicked);
    connect(linkerDefaultButton, &QPushButton::clicked, this, &MainWindow::onSetDefaultButtonClicked);

    // Add a status tip to each button
    compilerPathDefaultButton->setStatusTip("Set current Compiler Path as default");
    llvmDirDefaultButton->setStatusTip("Set current LLVM Directory as default");
    buildDirDefaultButton->setStatusTip("Set current Build Directory as default");
    installPathDefaultButton->setStatusTip("Set current Install Path as default");
    timerFileDefaultButton->setStatusTip("Set current Timer File as default");
    projectsDefaultButton->setStatusTip("Set current Projects as default");
    runtimesDefaultButton->setStatusTip("Set current Runtimes as default");
    compilerDefaultButton->setStatusTip("Set current Compiler as default");
    cxxCompilerDefaultButton->setStatusTip("Set current C++ Compiler as default");
    linkerDefaultButton->setStatusTip("Set current Linker as default");
}

void MainWindow::loadFieldDefaults()
{
    // Load defaults for each field
    if (FieldDefaultsManager::instance().hasFieldDefault("compilerPath")) {
        ui->compilerPathLineEdit->setText(FieldDefaultsManager::instance().getFieldDefault("compilerPath"));
    }

    if (FieldDefaultsManager::instance().hasFieldDefault("llvmDir")) {
        ui->llvmDirLineEdit->setText(FieldDefaultsManager::instance().getFieldDefault("llvmDir"));
    }

    if (FieldDefaultsManager::instance().hasFieldDefault("buildDir")) {
        ui->buildDirLineEdit->setText(FieldDefaultsManager::instance().getFieldDefault("buildDir"));
    }

    if (FieldDefaultsManager::instance().hasFieldDefault("installPath")) {
        ui->installPathLineEdit->setText(FieldDefaultsManager::instance().getFieldDefault("installPath"));
    }

    if (FieldDefaultsManager::instance().hasFieldDefault("timerFile")) {
        ui->timerFileLineEdit->setText(FieldDefaultsManager::instance().getFieldDefault("timerFile"));
    }

    if (FieldDefaultsManager::instance().hasFieldDefault("projects")) {
        ui->projectsLineEdit->setText(FieldDefaultsManager::instance().getFieldDefault("projects"));
        updateProjectCheckboxes(ui->projectsLineEdit->text());
    }

    if (FieldDefaultsManager::instance().hasFieldDefault("runtimes")) {
        ui->runtimesLineEdit->setText(FieldDefaultsManager::instance().getFieldDefault("runtimes"));
        updateRuntimeCheckboxes(ui->runtimesLineEdit->text());
    }

    if (FieldDefaultsManager::instance().hasFieldDefault("compiler")) {
        ui->compilerLineEdit->setText(FieldDefaultsManager::instance().getFieldDefault("compiler"));
    }

    if (FieldDefaultsManager::instance().hasFieldDefault("cxxCompiler")) {
        ui->cxxCompilerLineEdit->setText(FieldDefaultsManager::instance().getFieldDefault("cxxCompiler"));
    }

    if (FieldDefaultsManager::instance().hasFieldDefault("linker")) {
        ui->linkerLineEdit->setText(FieldDefaultsManager::instance().getFieldDefault("linker"));
    }

    // Update the configuration from the UI
    updateConfigFromUI();
}

void MainWindow::saveFieldDefault(const QString &fieldName, const QString &value)
{
    // Save the default value
    FieldDefaultsManager::instance().saveFieldDefault(fieldName, value);

    // Show a message to the user
    statusBar()->showMessage("Default value saved for " + fieldName, 3000);
}

void MainWindow::onSetDefaultButtonClicked()
{
    // Get the sender button
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) {
        return;
    }

    // Get the button's object name
    QString buttonName = button->objectName();

    // Determine which field to save based on the button name
    if (buttonName == "compilerPathDefaultButton") {
        saveFieldDefault("compilerPath", ui->compilerPathLineEdit->text());
    } else if (buttonName == "llvmDirDefaultButton") {
        saveFieldDefault("llvmDir", ui->llvmDirLineEdit->text());
    } else if (buttonName == "buildDirDefaultButton") {
        saveFieldDefault("buildDir", ui->buildDirLineEdit->text());
    } else if (buttonName == "installPathDefaultButton") {
        saveFieldDefault("installPath", ui->installPathLineEdit->text());
    } else if (buttonName == "timerFileDefaultButton") {
        saveFieldDefault("timerFile", ui->timerFileLineEdit->text());
    } else if (buttonName == "projectsDefaultButton") {
        saveFieldDefault("projects", ui->projectsLineEdit->text());
    } else if (buttonName == "runtimesDefaultButton") {
        saveFieldDefault("runtimes", ui->runtimesLineEdit->text());
    } else if (buttonName == "compilerDefaultButton") {
        saveFieldDefault("compiler", ui->compilerLineEdit->text());
    } else if (buttonName == "cxxCompilerDefaultButton") {
        saveFieldDefault("cxxCompiler", ui->cxxCompilerLineEdit->text());
    } else if (buttonName == "linkerDefaultButton") {
        saveFieldDefault("linker", ui->linkerLineEdit->text());
    }
}
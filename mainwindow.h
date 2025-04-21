#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QLineEdit>
#include <QPushButton>

class BuilderConfiguration;
class CommandGenerator;
class BuildExecutor;
class ConfigurationDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // UI event handlers
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionSave_Configuration_triggered();
    void on_actionLoad_Configuration_triggered();
    void on_actionReset_to_Defaults_triggered();

    void on_generateButton_clicked();
    void on_buildButton_clicked();
    void on_cancelButton_clicked();
    void on_clearOutputButton_clicked();
    void on_saveOutputButton_clicked();
    void on_copyCommandButton_clicked();

    void on_botModeCheckBox_toggled(bool checked);
    void on_dryRunCheckBox_toggled(bool checked);
    void on_doInstallCheckBox_toggled(bool checked);
    void on_noLtoCheckBox_toggled(bool checked);
    void on_fullLtoCheckBox_toggled(bool checked);

    void on_browseCompilerPathButton_clicked();
    void on_browseLlvmDirButton_clicked();
    void on_browseBuildDirButton_clicked();
    void on_browseInstallPathButton_clicked();
    void on_browseTimerFileButton_clicked();

    // Project and Runtime validation
    void on_projectsLineEdit_editingFinished();
    void on_runtimesLineEdit_editingFinished();

    // Project checkboxes
    void on_boltCheckBox_toggled(bool checked);
    void on_clangCheckBox_toggled(bool checked);
    void on_clangToolsExtraCheckBox_toggled(bool checked);
    void on_compilerRtProjectCheckBox_toggled(bool checked);
    void on_crossProjectTestsCheckBox_toggled(bool checked);
    void on_libcProjectCheckBox_toggled(bool checked);
    void on_libclcCheckBox_toggled(bool checked);
    void on_lldCheckBox_toggled(bool checked);
    void on_lldbCheckBox_toggled(bool checked);
    void on_mlirCheckBox_toggled(bool checked);
    void on_openmpProjectCheckBox_toggled(bool checked);
    void on_pollyCheckBox_toggled(bool checked);
    void on_pstlProjectCheckBox_toggled(bool checked);
    void on_flangCheckBox_toggled(bool checked);

    // Runtime checkboxes
    void on_libcRuntimeCheckBox_toggled(bool checked);
    void on_libunwindCheckBox_toggled(bool checked);
    void on_libcxxabiCheckBox_toggled(bool checked);
    void on_pstlRuntimeCheckBox_toggled(bool checked);
    void on_libcxxCheckBox_toggled(bool checked);
    void on_compilerRtRuntimeCheckBox_toggled(bool checked);
    void on_openmpRuntimeCheckBox_toggled(bool checked);
    void on_llvmLibgccCheckBox_toggled(bool checked);
    void on_offloadCheckBox_toggled(bool checked);

    // Build executor event handlers
    void onBuildStarted();
    void onBuildFinished(bool success, const QString &message);
    void onOutputAvailable(const QString &output);

private:
    Ui::MainWindow *ui;
    BuilderConfiguration *m_config;
    CommandGenerator *m_generator;
    BuildExecutor *m_executor;
    ConfigurationDialog *m_configDialog;

    // Update the UI from the configuration
    void updateUIFromConfig();

    // Update the configuration from the UI
    void updateConfigFromUI();

    // Enable/disable UI elements based on state
    void updateUIState(bool buildRunning);

    // Apply bot mode settings
    void applyBotModeSettings();

    // Helper methods for UI updates
    void updatePathFields();
    void updateCompilerFields();
    void updateBuildOptions();

    // Helper methods for browsing
    QString browseForDirectory(const QString &title, const QString &startDir);
    QString browseForFile(const QString &title, const QString &startDir, const QString &filter);

    // Helper method to validate projects and runtimes
    void validateProjectsAndRuntimes();

    // Helper method to check for duplicates between projects and runtimes
    QStringList findDuplicates(const QString &projects, const QString &runtimes);

    // Helper methods for checkbox handling
    void updateCheckboxesFromText();
    void updateProjectCheckboxes(const QString &projects);
    void updateRuntimeCheckboxes(const QString &runtimes);
    void updateTextFromCheckboxes();
    QString getProjectsFromCheckboxes();
    QString getRuntimesFromCheckboxes();
    void handleProjectCheckboxToggled(const QString &project, bool checked);
    void handleRuntimeCheckboxToggled(const QString &runtime, bool checked);

    // Field defaults handling
    void setupDefaultButtons();
    void loadFieldDefaults();
    void saveFieldDefault(const QString &fieldName, const QString &value);

private slots:
    // Default button slots
    void onSetDefaultButtonClicked();
};

#endif // MAINWINDOW_H

#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>
#include <QMap>
#include <QString>

namespace Ui {
class ConfigurationDialog;
}

class BuilderConfiguration;

class ConfigurationDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ConfigurationDialog(QWidget *parent = nullptr);
    ~ConfigurationDialog();
    
    // Save the current configuration
    void saveConfiguration(const BuilderConfiguration &config, const QString &name);
    
    // Load a saved configuration
    BuilderConfiguration loadConfiguration(const QString &name) const;
    
    // Get a list of saved configurations
    QStringList savedConfigurations() const;
    
    // Delete a saved configuration
    void deleteConfiguration(const QString &name);
    
private slots:
    void on_saveButton_clicked();
    void on_loadButton_clicked();
    void on_deleteButton_clicked();
    void on_configList_currentTextChanged(const QString &text);
    
private:
    Ui::ConfigurationDialog *ui;
    
    // Map of configuration names to file paths
    QMap<QString, QString> m_configFiles;
    
    // Directory for storing configurations
    QString m_configDir;
    
    // Load the list of saved configurations
    void loadConfigurationList();
    
    // Get the file path for a configuration
    QString configFilePath(const QString &name) const;
};

#endif // CONFIGURATIONDIALOG_H

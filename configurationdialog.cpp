#include "configurationdialog.h"
#include "ui_configurationdialog.h"
#include "builderconfiguration.h"

#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileInfo>

ConfigurationDialog::ConfigurationDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConfigurationDialog)
{
    ui->setupUi(this);
    
    // Set up the configuration directory
    m_configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/configurations";
    QDir dir(m_configDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // Load the list of saved configurations
    loadConfigurationList();
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}

void ConfigurationDialog::saveConfiguration(const BuilderConfiguration &config, const QString &name)
{
    // Create the file path
    QString filePath = configFilePath(name);
    
    // Save the configuration
    if (config.saveToFile(filePath)) {
        // Add to the list if it's not already there
        if (!m_configFiles.contains(name)) {
            m_configFiles[name] = filePath;
            ui->configList->addItem(name);
        }
    } else {
        QMessageBox::warning(this, "Save Error", "Failed to save configuration: " + name);
    }
}

BuilderConfiguration ConfigurationDialog::loadConfiguration(const QString &name) const
{
    BuilderConfiguration config;
    
    // Check if the configuration exists
    if (!m_configFiles.contains(name)) {
        return config;
    }
    
    // Load the configuration
    QString filePath = m_configFiles[name];
    if (!config.loadFromFile(filePath)) {
        QMessageBox::warning(const_cast<ConfigurationDialog*>(this), "Load Error", 
                            "Failed to load configuration: " + name);
    }
    
    return config;
}

QStringList ConfigurationDialog::savedConfigurations() const
{
    return m_configFiles.keys();
}

void ConfigurationDialog::deleteConfiguration(const QString &name)
{
    // Check if the configuration exists
    if (!m_configFiles.contains(name)) {
        return;
    }
    
    // Delete the file
    QString filePath = m_configFiles[name];
    QFile file(filePath);
    if (file.remove()) {
        // Remove from the list
        m_configFiles.remove(name);
        
        // Update the UI
        QList<QListWidgetItem*> items = ui->configList->findItems(name, Qt::MatchExactly);
        for (QListWidgetItem *item : items) {
            delete ui->configList->takeItem(ui->configList->row(item));
        }
    } else {
        QMessageBox::warning(this, "Delete Error", "Failed to delete configuration: " + name);
    }
}

void ConfigurationDialog::loadConfigurationList()
{
    // Clear the current list
    ui->configList->clear();
    m_configFiles.clear();
    
    // Get all JSON files in the configuration directory
    QDir dir(m_configDir);
    QStringList filters;
    filters << "*.json";
    QStringList files = dir.entryList(filters, QDir::Files);
    
    // Add each file to the list
    for (const QString &file : files) {
        QString name = QFileInfo(file).baseName();
        QString filePath = dir.filePath(file);
        m_configFiles[name] = filePath;
        ui->configList->addItem(name);
    }
}

QString ConfigurationDialog::configFilePath(const QString &name) const
{
    return m_configDir + "/" + name + ".json";
}

void ConfigurationDialog::on_saveButton_clicked()
{
    // Get the configuration name from the user
    bool ok;
    QString name = QInputDialog::getText(this, "Save Configuration", 
                                        "Configuration Name:", QLineEdit::Normal, 
                                        "", &ok);
    if (!ok || name.isEmpty()) {
        return;
    }
    
    // Check if the configuration already exists
    if (m_configFiles.contains(name)) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Overwrite Configuration", 
                                                                "A configuration with this name already exists. Overwrite?", 
                                                                QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            return;
        }
    }
    
    // Emit the save signal
    emit accept();
}

void ConfigurationDialog::on_loadButton_clicked()
{
    // Get the selected configuration
    QString name = ui->configList->currentItem()->text();
    if (name.isEmpty()) {
        return;
    }
    
    // Emit the load signal
    emit accept();
}

void ConfigurationDialog::on_deleteButton_clicked()
{
    // Get the selected configuration
    QListWidgetItem *item = ui->configList->currentItem();
    if (!item) {
        return;
    }
    
    QString name = item->text();
    
    // Confirm deletion
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete Configuration", 
                                                            "Are you sure you want to delete the configuration: " + name + "?", 
                                                            QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }
    
    // Delete the configuration
    deleteConfiguration(name);
}

void ConfigurationDialog::on_configList_currentTextChanged(const QString &text)
{
    // Enable/disable buttons based on selection
    bool hasSelection = !text.isEmpty();
    ui->loadButton->setEnabled(hasSelection);
    ui->deleteButton->setEnabled(hasSelection);
}

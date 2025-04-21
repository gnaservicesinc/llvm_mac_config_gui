#ifndef COMMANDGENERATOR_H
#define COMMANDGENERATOR_H

#include "builderconfiguration.h"
#include <QString>

class CommandGenerator
{
public:
    CommandGenerator(const BuilderConfiguration &config);
    
    // Generate the full build command
    QString generateBuildCommand() const;
    
    // Generate just the CMake configuration command
    QString generateCMakeCommand() const;
    
    // Generate the build execution command (ninja or make)
    QString generateBuildExecutionCommand() const;
    
    // Generate the install command
    QString generateInstallCommand() const;
    
    // Generate the full script that would be equivalent to buildersalone.sh
    QString generateFullScript() const;
    
private:
    const BuilderConfiguration &m_config;
};

#endif // COMMANDGENERATOR_H

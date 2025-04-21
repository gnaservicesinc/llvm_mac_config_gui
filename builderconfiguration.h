#ifndef BUILDERCONFIGURATION_H
#define BUILDERCONFIGURATION_H

#include <QString>
#include <QMap>
#include <QVariant>
#include <QJsonObject>

class BuilderConfiguration
{
public:
    BuilderConfiguration();
    
    // Path settings
    QString compilerPath() const;
    void setCompilerPath(const QString &path);
    
    QString llvmDir() const;
    void setLlvmDir(const QString &path);
    
    QString buildDir() const;
    void setBuildDir(const QString &path);
    
    QString installPath() const;
    void setInstallPath(const QString &path);
    
    QString timerFile() const;
    void setTimerFile(const QString &path);
    
    // Compiler settings
    QString projects() const;
    void setProjects(const QString &projects);
    
    QString runtimes() const;
    void setRuntimes(const QString &runtimes);
    
    bool cleanBuildDir() const;
    void setCleanBuildDir(bool clean);
    
    QString compiler() const;
    void setCompiler(const QString &compiler);
    
    QString cxxCompiler() const;
    void setCxxCompiler(const QString &compiler);
    
    QString linker() const;
    void setLinker(const QString &linker);
    
    QString optLevel() const;
    void setOptLevel(const QString &level);
    
    QString arch() const;
    void setArch(const QString &arch);
    
    QString osxArch() const;
    void setOsxArch(const QString &arch);
    
    // Build options
    bool dryRun() const;
    void setDryRun(bool dryRun);
    
    bool ffi() const;
    void setFfi(bool enabled);
    
    bool zlib() const;
    void setZlib(bool enabled);
    
    bool terminfo() const;
    void setTerminfo(bool enabled);
    
    bool xml2() const;
    void setXml2(bool enabled);
    
    bool noLto() const;
    void setNoLto(bool disabled);
    
    bool fullLto() const;
    void setFullLto(bool enabled);
    
    bool useMake() const;
    void setUseMake(bool useMake);
    
    bool skipGitPull() const;
    void setSkipGitPull(bool skip);
    
    bool doInstall() const;
    void setDoInstall(bool install);
    
    bool sudoInstall() const;
    void setSudoInstall(bool sudo);
    
    bool useLocalPython() const;
    void setUseLocalPython(bool local);
    
    bool useDylib() const;
    void setUseDylib(bool dylib);
    
    bool xcodeToolchain() const;
    void setXcodeToolchain(bool toolchain);
    
    bool useXcodeGcc() const;
    void setUseXcodeGcc(bool gcc);
    
    bool modules() const;
    void setModules(bool enabled);
    
    bool backtraces() const;
    void setBacktraces(bool enabled);
    
    bool doNotWarn() const;
    void setDoNotWarn(bool noWarn);
    
    bool doTesting() const;
    void setDoTesting(bool testing);
    
    bool benchmark() const;
    void setBenchmark(bool benchmark);
    
    bool botMode() const;
    void setBotMode(bool bot);
    
    // Save/load configuration
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);
    
    bool saveToFile(const QString &filePath) const;
    bool loadFromFile(const QString &filePath);
    
    // Reset to defaults
    void resetToDefaults();
    
private:
    // Path settings
    QString m_compilerPath;
    QString m_llvmDir;
    QString m_buildDir;
    QString m_installPath;
    QString m_timerFile;
    
    // Compiler settings
    QString m_projects;
    QString m_runtimes;
    bool m_cleanBuildDir;
    QString m_compiler;
    QString m_cxxCompiler;
    QString m_linker;
    QString m_optLevel;
    QString m_arch;
    QString m_osxArch;
    
    // Build options
    bool m_dryRun;
    bool m_ffi;
    bool m_zlib;
    bool m_terminfo;
    bool m_xml2;
    bool m_noLto;
    bool m_fullLto;
    bool m_useMake;
    bool m_skipGitPull;
    bool m_doInstall;
    bool m_sudoInstall;
    bool m_useLocalPython;
    bool m_useDylib;
    bool m_xcodeToolchain;
    bool m_useXcodeGcc;
    bool m_modules;
    bool m_backtraces;
    bool m_doNotWarn;
    bool m_doTesting;
    bool m_benchmark;
    bool m_botMode;
};

#endif // BUILDERCONFIGURATION_H

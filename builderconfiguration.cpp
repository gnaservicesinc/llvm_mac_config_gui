#include "builderconfiguration.h"
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QTemporaryDir>

BuilderConfiguration::BuilderConfiguration()
{
    resetToDefaults();
}

void BuilderConfiguration::resetToDefaults()
{
    // Path settings
    m_compilerPath = "/usr/local";
    m_llvmDir = "/opt/llvm/llvm-project";
    m_buildDir = QDir::tempPath();
    m_installPath = "/usr/local";
    m_timerFile = QDir::currentPath() + "/time.txt";

    // Compiler settings
    m_projects = "bolt;clang;clang-tools-extra;compiler-rt;cross-project-tests;libc;libclc;lld;lldb;mlir;openmp;polly;pstl;flang";
    m_runtimes = "";
    m_cleanBuildDir = true;
    m_compiler = "clang";
    m_cxxCompiler = "clang++";
    m_linker = "ld64.lld";
    m_optLevel = "2";
    m_arch = "AArch64";
    m_osxArch = "arm64";

    // Build options
    m_dryRun = false;
    m_ffi = false;
    m_zlib = false;
    m_terminfo = false;
    m_xml2 = false;
    m_noLto = false;
    m_fullLto = false;
    m_useMake = false;
    m_skipGitPull = false;
    m_doInstall = false;
    m_sudoInstall = false;
    m_useLocalPython = false;
    m_useDylib = false;
    m_xcodeToolchain = false;
    m_useXcodeGcc = false;
    m_modules = false;
    m_backtraces = false;
    m_doNotWarn = false;
    m_doTesting = false;
    m_benchmark = false;
    m_botMode = false;
}

// Path settings
QString BuilderConfiguration::compilerPath() const { return m_compilerPath; }
void BuilderConfiguration::setCompilerPath(const QString &path) { m_compilerPath = path; }

QString BuilderConfiguration::llvmDir() const { return m_llvmDir; }
void BuilderConfiguration::setLlvmDir(const QString &path) { m_llvmDir = path; }

QString BuilderConfiguration::buildDir() const { return m_buildDir; }
void BuilderConfiguration::setBuildDir(const QString &path) { m_buildDir = path; }

QString BuilderConfiguration::installPath() const { return m_installPath; }
void BuilderConfiguration::setInstallPath(const QString &path) { m_installPath = path; }

QString BuilderConfiguration::timerFile() const { return m_timerFile; }
void BuilderConfiguration::setTimerFile(const QString &path) { m_timerFile = path; }

// Compiler settings
QString BuilderConfiguration::projects() const { return m_projects; }
void BuilderConfiguration::setProjects(const QString &projects) { m_projects = projects; }

QString BuilderConfiguration::runtimes() const { return m_runtimes; }
void BuilderConfiguration::setRuntimes(const QString &runtimes) { m_runtimes = runtimes; }

bool BuilderConfiguration::cleanBuildDir() const { return m_cleanBuildDir; }
void BuilderConfiguration::setCleanBuildDir(bool clean) { m_cleanBuildDir = clean; }

QString BuilderConfiguration::compiler() const { return m_compiler; }
void BuilderConfiguration::setCompiler(const QString &compiler) { m_compiler = compiler; }

QString BuilderConfiguration::cxxCompiler() const { return m_cxxCompiler; }
void BuilderConfiguration::setCxxCompiler(const QString &compiler) { m_cxxCompiler = compiler; }

QString BuilderConfiguration::linker() const { return m_linker; }
void BuilderConfiguration::setLinker(const QString &linker) { m_linker = linker; }

QString BuilderConfiguration::optLevel() const { return m_optLevel; }
void BuilderConfiguration::setOptLevel(const QString &level) { m_optLevel = level; }

QString BuilderConfiguration::arch() const { return m_arch; }
void BuilderConfiguration::setArch(const QString &arch) { m_arch = arch; }

QString BuilderConfiguration::osxArch() const { return m_osxArch; }
void BuilderConfiguration::setOsxArch(const QString &arch) { m_osxArch = arch; }

// Build options
bool BuilderConfiguration::dryRun() const { return m_dryRun; }
void BuilderConfiguration::setDryRun(bool dryRun) { m_dryRun = dryRun; }

bool BuilderConfiguration::ffi() const { return m_ffi; }
void BuilderConfiguration::setFfi(bool enabled) { m_ffi = enabled; }

bool BuilderConfiguration::zlib() const { return m_zlib; }
void BuilderConfiguration::setZlib(bool enabled) { m_zlib = enabled; }

bool BuilderConfiguration::terminfo() const { return m_terminfo; }
void BuilderConfiguration::setTerminfo(bool enabled) { m_terminfo = enabled; }

bool BuilderConfiguration::xml2() const { return m_xml2; }
void BuilderConfiguration::setXml2(bool enabled) { m_xml2 = enabled; }

bool BuilderConfiguration::noLto() const { return m_noLto; }
void BuilderConfiguration::setNoLto(bool disabled) { m_noLto = disabled; }

bool BuilderConfiguration::fullLto() const { return m_fullLto; }
void BuilderConfiguration::setFullLto(bool enabled) { m_fullLto = enabled; }

bool BuilderConfiguration::useMake() const { return m_useMake; }
void BuilderConfiguration::setUseMake(bool useMake) { m_useMake = useMake; }

bool BuilderConfiguration::skipGitPull() const { return m_skipGitPull; }
void BuilderConfiguration::setSkipGitPull(bool skip) { m_skipGitPull = skip; }

bool BuilderConfiguration::doInstall() const { return m_doInstall; }
void BuilderConfiguration::setDoInstall(bool install) { m_doInstall = install; }

bool BuilderConfiguration::sudoInstall() const { return m_sudoInstall; }
void BuilderConfiguration::setSudoInstall(bool sudo) { m_sudoInstall = sudo; }

bool BuilderConfiguration::useLocalPython() const { return m_useLocalPython; }
void BuilderConfiguration::setUseLocalPython(bool local) { m_useLocalPython = local; }

bool BuilderConfiguration::useDylib() const { return m_useDylib; }
void BuilderConfiguration::setUseDylib(bool dylib) { m_useDylib = dylib; }

bool BuilderConfiguration::xcodeToolchain() const { return m_xcodeToolchain; }
void BuilderConfiguration::setXcodeToolchain(bool toolchain) { m_xcodeToolchain = toolchain; }

bool BuilderConfiguration::useXcodeGcc() const { return m_useXcodeGcc; }
void BuilderConfiguration::setUseXcodeGcc(bool gcc) { m_useXcodeGcc = gcc; }

bool BuilderConfiguration::modules() const { return m_modules; }
void BuilderConfiguration::setModules(bool enabled) { m_modules = enabled; }

bool BuilderConfiguration::backtraces() const { return m_backtraces; }
void BuilderConfiguration::setBacktraces(bool enabled) { m_backtraces = enabled; }

bool BuilderConfiguration::doNotWarn() const { return m_doNotWarn; }
void BuilderConfiguration::setDoNotWarn(bool noWarn) { m_doNotWarn = noWarn; }

bool BuilderConfiguration::doTesting() const { return m_doTesting; }
void BuilderConfiguration::setDoTesting(bool testing) { m_doTesting = testing; }

bool BuilderConfiguration::benchmark() const { return m_benchmark; }
void BuilderConfiguration::setBenchmark(bool benchmark) { m_benchmark = benchmark; }

bool BuilderConfiguration::botMode() const { return m_botMode; }
void BuilderConfiguration::setBotMode(bool bot) { m_botMode = bot; }

QJsonObject BuilderConfiguration::toJson() const
{
    QJsonObject json;

    // Path settings
    json["compilerPath"] = m_compilerPath;
    json["llvmDir"] = m_llvmDir;
    json["buildDir"] = m_buildDir;
    json["installPath"] = m_installPath;
    json["timerFile"] = m_timerFile;

    // Compiler settings
    json["projects"] = m_projects;
    json["runtimes"] = m_runtimes;
    json["cleanBuildDir"] = m_cleanBuildDir;
    json["compiler"] = m_compiler;
    json["cxxCompiler"] = m_cxxCompiler;
    json["linker"] = m_linker;
    json["optLevel"] = m_optLevel;
    json["arch"] = m_arch;
    json["osxArch"] = m_osxArch;

    // Build options
    json["dryRun"] = m_dryRun;
    json["ffi"] = m_ffi;
    json["zlib"] = m_zlib;
    json["terminfo"] = m_terminfo;
    json["xml2"] = m_xml2;
    json["noLto"] = m_noLto;
    json["fullLto"] = m_fullLto;
    json["useMake"] = m_useMake;
    json["skipGitPull"] = m_skipGitPull;
    json["doInstall"] = m_doInstall;
    json["sudoInstall"] = m_sudoInstall;
    json["useLocalPython"] = m_useLocalPython;
    json["useDylib"] = m_useDylib;
    json["xcodeToolchain"] = m_xcodeToolchain;
    json["useXcodeGcc"] = m_useXcodeGcc;
    json["modules"] = m_modules;
    json["backtraces"] = m_backtraces;
    json["doNotWarn"] = m_doNotWarn;
    json["doTesting"] = m_doTesting;
    json["benchmark"] = m_benchmark;
    json["botMode"] = m_botMode;

    return json;
}

void BuilderConfiguration::fromJson(const QJsonObject &json)
{
    // Path settings
    if (json.contains("compilerPath")) m_compilerPath = json["compilerPath"].toString();
    if (json.contains("llvmDir")) m_llvmDir = json["llvmDir"].toString();
    if (json.contains("buildDir")) m_buildDir = json["buildDir"].toString();
    if (json.contains("installPath")) m_installPath = json["installPath"].toString();
    if (json.contains("timerFile")) m_timerFile = json["timerFile"].toString();

    // Compiler settings
    if (json.contains("projects")) m_projects = json["projects"].toString();
    if (json.contains("runtimes")) m_runtimes = json["runtimes"].toString();
    if (json.contains("cleanBuildDir")) m_cleanBuildDir = json["cleanBuildDir"].toBool();
    if (json.contains("compiler")) m_compiler = json["compiler"].toString();
    if (json.contains("cxxCompiler")) m_cxxCompiler = json["cxxCompiler"].toString();
    if (json.contains("linker")) m_linker = json["linker"].toString();
    if (json.contains("optLevel")) m_optLevel = json["optLevel"].toString();
    if (json.contains("arch")) m_arch = json["arch"].toString();
    if (json.contains("osxArch")) m_osxArch = json["osxArch"].toString();

    // Build options
    if (json.contains("dryRun")) m_dryRun = json["dryRun"].toBool();
    if (json.contains("ffi")) m_ffi = json["ffi"].toBool();
    if (json.contains("zlib")) m_zlib = json["zlib"].toBool();
    if (json.contains("terminfo")) m_terminfo = json["terminfo"].toBool();
    if (json.contains("xml2")) m_xml2 = json["xml2"].toBool();
    if (json.contains("noLto")) m_noLto = json["noLto"].toBool();
    if (json.contains("fullLto")) m_fullLto = json["fullLto"].toBool();
    if (json.contains("useMake")) m_useMake = json["useMake"].toBool();
    if (json.contains("skipGitPull")) m_skipGitPull = json["skipGitPull"].toBool();
    if (json.contains("doInstall")) m_doInstall = json["doInstall"].toBool();
    if (json.contains("sudoInstall")) m_sudoInstall = json["sudoInstall"].toBool();
    if (json.contains("useLocalPython")) m_useLocalPython = json["useLocalPython"].toBool();
    if (json.contains("useDylib")) m_useDylib = json["useDylib"].toBool();
    if (json.contains("xcodeToolchain")) m_xcodeToolchain = json["xcodeToolchain"].toBool();
    if (json.contains("useXcodeGcc")) m_useXcodeGcc = json["useXcodeGcc"].toBool();
    if (json.contains("modules")) m_modules = json["modules"].toBool();
    if (json.contains("backtraces")) m_backtraces = json["backtraces"].toBool();
    if (json.contains("doNotWarn")) m_doNotWarn = json["doNotWarn"].toBool();
    if (json.contains("doTesting")) m_doTesting = json["doTesting"].toBool();
    if (json.contains("benchmark")) m_benchmark = json["benchmark"].toBool();
    if (json.contains("botMode")) m_botMode = json["botMode"].toBool();
}

bool BuilderConfiguration::saveToFile(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument doc(toJson());
    file.write(doc.toJson());
    return true;
}

bool BuilderConfiguration::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull() || !doc.isObject()) {
        return false;
    }

    fromJson(doc.object());
    return true;
}

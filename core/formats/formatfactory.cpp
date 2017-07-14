#include "formatfactory.h"
#include <QFileInfo>
#include <QObject>

#include "files/csvfile.h"
#include "files/htmlfile.h"
#include "files/mpbfile.h"
#include "files/nbffile.h"
#include "files/udxfile.h"
#include "files/vcffile.h"

#include "QDebug"

FormatFactory::FormatFactory()
    :error("")
{
}

QStringList FormatFactory::supportedFilters(QIODevice::OpenMode mode, bool isReportFormat)
{
    QStringList allTypes;
    // Known formats (all supported)
    QString allSupported;
    if (isReportFormat){
        allSupported += "*." + HTMLFile::supportedExtensions().join(" *.");
        allTypes << HTMLFile::supportedFilters();
    }
    else {
        allSupported += " *." + VCFFile::supportedExtensions().join(" *.");
        allSupported += " *." + UDXFile::supportedExtensions().join(" *.");
#if QT_VERSION >= 0x040800
        allSupported += " *." + MPBFile::supportedExtensions().join(" *.");
#else
#warning MPB save and load will not work under Qt earlier than 4.8
#endif
        allSupported += " *." + CSVFile::supportedExtensions().join(" *.");
        if (mode==QIODevice::ReadOnly) {
            // ...here add read-only formats
            allSupported += " *." + NBFFile::supportedExtensions().join(" *.");
        }
        else { // Write-only formats
        }
        qDebug() << allSupported;
        // ...here add supportedExtensions() for new format
        allTypes << S_ALL_SUPPORTED.arg(allSupported);
        // Known formats (separate)
        allTypes << VCFFile::supportedFilters();
        allTypes << UDXFile::supportedFilters();
#if QT_VERSION >= 0x040800
        allTypes << MPBFile::supportedFilters();
#endif
        allTypes << CSVFile::supportedFilters();
        if (mode==QIODevice::ReadOnly) {
            // ...here add filters for read-only formats
            allTypes << NBFFile::supportedFilters();
        }
        else { // Write-only formats
        }
        // ...here add supportedFilters() for new format
    }
    allTypes << S_ALL_FILES;
    return allTypes;
}

IFormat *FormatFactory::createObject(const QString &url)
{
    if (url.isEmpty()) {
        error = QObject::tr("Empty file name");
        return 0;
    }
    QFileInfo info(url);
    QString ext = info.completeSuffix();
    // Known formats by extension
    if (VCFFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new VCFFile();
    if (UDXFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new UDXFile();
    if (CSVFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new CSVFile();
    if (NBFFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new NBFFile();
#if QT_VERSION >= 0x040800
    if (MPBFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new MPBFile();
#endif
    if (HTMLFile::supportedExtensions().contains(ext, Qt::CaseInsensitive))
        return new HTMLFile();
    // ...here add supportedExtensions() for new format
    // Known formats with non-standard extension
    if (VCFFile::detect(url))
        return new VCFFile();
    if (UDXFile::detect(url))
        return new UDXFile();
#if QT_VERSION >= 0x040800
    if (MPBFile::detect(url))
        return new MPBFile();
#endif
    if (NBFFile::detect(url))
        return new NBFFile();
    if (CSVFile::detect(url))
        return new CSVFile();
    // ...here add detect() for new format
    // Sad but true
    error = QObject::tr("Unknown file format:\n%1").arg(url);
    return 0;
}

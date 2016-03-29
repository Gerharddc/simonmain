#include "filebrowser.h"

#include <QStringList>

FileBrowser::FileBrowser(QObject *parent) : QObject(parent)
{
    curDir = QDir(rootDir);
}

void FileBrowser::GetFilesForDir()
{
    // Add the directories
    for (QString str : curDir.entryList(QStringList(""), QDir::AllDirs | QDir::NoDot | QDir::NoDotDot, QDir::Name))
    {
        QVariantMap mp;
        mp.insert("displayName", str);
        mp.insert("glyph", "Folder.png");
        mp.insert("cd", str);
        mp.insert("open", "");
        m_fileModel.append(mp);
    }

    // Add the files
    for (QString str : curDir.entryList(QStringList("*.stl"), QDir::Files, QDir::Name))
    {
        QVariantMap mp;
        mp.insert("displayName", str);
        mp.insert("glyph", "STL.png");
        mp.insert("cd", "");
        mp.insert("open", curDir.absoluteFilePath(str));
        m_fileModel.append(mp);
    }
}

void FileBrowser::getRootDirectory()
{
    isRootDir = true;

    // Resert the dir and clear the lsit
    curDir.setPath(rootDir);
    m_fileModel.clear();

    GetFilesForDir();

    // Add the external drives
    // TODO: do this properly
    QVariantMap sd;
    sd.insert("displayName", "SD Card");
    sd.insert("glyph", "SD.png");
    sd.insert("cd", ".mnt/SD");
    sd.insert("open", "");
    m_fileModel.append(sd);

    QVariantMap usb;
    usb.insert("displayName", "USD Drive");
    usb.insert("glyph", "USB.png");
    usb.insert("cd", ".mnt/USB");
    usb.insert("open", "");
    m_fileModel.append(usb);

    emit fileModelChanged();
}

void FileBrowser::reset()
{
    // TODO: maybe something else?
    getRootDirectory();
}

void FileBrowser::selectItem(QString cd)
{
    // Open dir
    curDir.cd(cd);
    if (curDir.absolutePath() == rootDir || curDir.dirName() == ".mnt")
        getRootDirectory();
    else
    {
        m_fileModel.clear();

        // Add the aprent directory entry
        QVariantMap mp;
        mp.insert("displayName", "Back");
        mp.insert("glyph", "Up.png");
        mp.insert("cd", "../");
        mp.insert("open", "");
        m_fileModel.append(mp);

        GetFilesForDir();

        emit fileModelChanged();
    }
}

#ifndef SEARCHCONTROLLER_H
#define SEARCHCONTROLLER_H

#include "abstractfilecontroller.h"

#include <QSet>

class SearchController : AbstractFileController
{
    Q_OBJECT

public:
    explicit SearchController(QObject *parent = 0);

    const QList<AbstractFileInfoPointer> getChildren(const DUrl &fileUrl, QDir::Filters filter, bool &accepted) const Q_DECL_OVERRIDE;
    const AbstractFileInfoPointer createFileInfo(const DUrl &fileUrl, bool &accepted) const Q_DECL_OVERRIDE;
    bool openFileLocation(const DUrl &fileUrl, bool &accepted) const Q_DECL_OVERRIDE;

    bool openFile(const DUrl &fileUrl, bool &accepted) const Q_DECL_OVERRIDE;
    bool addUrlMonitor(const DUrl &fileUrl, bool &accepted) const Q_DECL_OVERRIDE;
    bool removeUrlMonitor(const DUrl &url, bool &accepted) const Q_DECL_OVERRIDE;
    bool copyFiles(const DUrlList &urlList, bool &accepted) const Q_DECL_OVERRIDE;
    bool moveToTrash(const DUrlList &urlList, bool &accepted) const Q_DECL_OVERRIDE;
    bool cutFiles(const DUrlList &urlList, bool &accepted) const Q_DECL_OVERRIDE;
    bool deleteFiles(const DUrlList &urlList, const FMEvent &event, bool &accepted) const Q_DECL_OVERRIDE;
    bool renameFile(const DUrl &oldUrl, const DUrl &newUrl, bool &accepted) const Q_DECL_OVERRIDE;

    bool compressFiles(const DUrlList &urlList, bool &accepted) const Q_DECL_OVERRIDE;
    bool decompressFile(const DUrl &fileUrl, bool &accepted) const Q_DECL_OVERRIDE;

    bool createSymlink(const DUrl &fileUrl, const DUrl &linkToUrl, bool &accepted) const Q_DECL_OVERRIDE;

    bool openInTerminal(const DUrl &fileUrl, bool &accepted) const Q_DECL_OVERRIDE;

private:
    void searchStart(const DUrl &fileUrl, QDir::Filters filter);
    static DUrl realUrl(const DUrl &searchUrl);
    static DUrlList realUrlList(const DUrlList &searchUrls);

    mutable QSet<DUrl> activeJob;
};

#endif // SEARCHCONTROLLER_H

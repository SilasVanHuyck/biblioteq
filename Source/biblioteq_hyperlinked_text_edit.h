#ifndef _BIBLIOTEQ_HYPERLINKED_TEXT_EDIT_H_
#define _BIBLIOTEQ_HYPERLINKED_TEXT_EDIT_H_

#include <QTextBrowser>
#include <QUrl>

#include <iostream>

class biblioteq_hyperlinked_text_edit: public QTextBrowser
{
  Q_OBJECT

 public:
  biblioteq_hyperlinked_text_edit(QWidget *parent);
  void setMultipleLinks(const QString &searchType,
			const QString &searchField,
			const QString &str);

 private slots:
  void slotAnchorClicked(const QUrl &url);
};

#endif

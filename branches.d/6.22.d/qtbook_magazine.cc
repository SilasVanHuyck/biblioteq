/*
** Includes magazine-specific methods.
*/

/*
** Note: The ISSN is not a unique value.
*/

/*
** -- Local Includes --
*/

#include "ui_locresults.h"
#include "qtbook_magazine.h"

extern qtbook *qmain;
extern QApplication *qapp;

/*
** -- qtbook_magazine() --
*/

qtbook_magazine::qtbook_magazine(QMainWindow *parentArg,
				 const QStringList &languages,
				 const QStringList &monetary_units,
				 const QStringList &locations,
				 const QString &oidArg,
				 const int rowArg):
  QMainWindow()
{
  QMenu *menu = 0;
  QRegExp rx("[0-9][0-9][0-9][0-9]-[0-9][0-9][0-9][0-9X]");
  QValidator *validator1 = 0;
  QGraphicsScene *scene1 = 0;
  QGraphicsScene *scene2 = 0;

  if((menu = new(std::nothrow) QMenu()) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((validator1 = new(std::nothrow) QRegExpValidator(rx, this)) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene1 = new(std::nothrow) QGraphicsScene()) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  if((scene2 = new(std::nothrow) QGraphicsScene()) == 0)
    qtbook::quit("Memory allocation failure", __FILE__, __LINE__);

  oid = oidArg;
  row = rowArg;
  subType = "Magazine";
  parentWid = parentArg;
  oldq = misc_functions::getColumnString
    (qmain->getUI().table, row, tr("Quantity")).toInt();
  ma.setupUi(this);
  updateFont(qapp->font(), static_cast<QWidget *> (this));
  connect(ma.okButton, SIGNAL(clicked(void)), this, SLOT(slotGo(void)));
  connect(ma.showUserButton, SIGNAL(clicked(void)), this,
	  SLOT(slotShowUsers(void)));
  connect(ma.queryButton, SIGNAL(clicked(void)), this,
	  SLOT(slotQuery(void)));
  connect(ma.cancelButton, SIGNAL(clicked(void)), this,
	  SLOT(slotCancel(void)));
  connect(ma.copiesButton, SIGNAL(clicked(void)), this,
	  SLOT(slotPopulateCopiesEditor(void)));
  connect(ma.resetButton, SIGNAL(clicked(void)), this,
	  SLOT(slotReset(void)));
  connect(ma.printButton, SIGNAL(clicked(void)), this, SLOT(slotPrint(void)));
  connect(menu->addAction(tr("Reset &Front Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Back Cover Image")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &ISSN")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Volume")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Issue")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &LC Control Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Call Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Dewey Number")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Title")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Publication Date")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Publisher")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Place of Publication")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Categories")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Price")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Language")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Monetary Units")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Copies")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Location")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &Abstract")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(menu->addAction(tr("Reset &OFFSYSTEM URL")),
	  SIGNAL(triggered(void)), this, SLOT(slotReset(void)));
  connect(ma.frontButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  connect(ma.backButton,
	  SIGNAL(clicked(void)), this, SLOT(slotSelectImage(void)));
  ma.id->setCursorPosition(0);
  ma.id->setValidator(validator1);
  ma.resetButton->setMenu(menu);
  ma.language->addItems(languages);
  ma.monetary_units->addItems(monetary_units);
  ma.location->addItems(locations);
  ma.front_image->setScene(scene1);
  ma.back_image->setScene(scene2);

  if(ma.language->count() == 0)
    ma.language->addItem(tr("UNKNOWN"));

  if(ma.monetary_units->count() == 0)
    ma.monetary_units->addItem(tr("UNKNOWN"));

  if(ma.location->count() == 0)
    ma.location->addItem(tr("UNKNOWN"));

  /*
  ** Save some palettes and style sheets.
  */

  dt_orig_ss = ma.publication_date->styleSheet();
  cb_orig_pal = ma.language->palette();
  te_orig_pal = ma.description->viewport()->palette();

  /*
  ** Prepare the form.
  */

  resize(baseSize());
  misc_functions::center(this, parentWid);
  misc_functions::hideAdminFields(this, qmain->getRoles());
}

/*
** -- ~qtbook_magazine() --
*/

qtbook_magazine::~qtbook_magazine()
{
}

/*
** -- slotGo() --
*/

void qtbook_magazine::slotGo(void)
{
  int i = 0;
  int newq = 0;
  int maxcopynumber = 0;
  QString str = "";
  QString errorstr = "";
  QString searchstr = "";
  QSqlQuery query(qmain->getDB());
  QTableWidgetItem *column = 0;

  if(engWindowTitle.contains("Create") ||
     engWindowTitle.contains("Modify"))
    {
      if(engWindowTitle.contains("Modify") && row > -1)
	{
	  newq = ma.quantity->value();
	  qapp->setOverrideCursor(Qt::WaitCursor);
	  maxcopynumber = misc_functions::getMaxCopyNumber
	    (qmain->getDB(), oid, subType, errorstr);

	  if(maxcopynumber < 0)
	    {
	      qapp->restoreOverrideCursor();
	      qmain->addError
		(QString(tr("Database Error")),
		 QString(tr("Unable to determine the maximum copy number of "
			    "the item.")),
		 errorstr, __FILE__, __LINE__);
	      QMessageBox::critical
		(this, tr("BiblioteQ: Database Error"),
		 tr("Unable to determine the maximum copy number of "
		    "the item."));
	      return;
	    }

	  qapp->restoreOverrideCursor();

	  if(newq < maxcopynumber)
	    {
	      QMessageBox::critical
		(this, tr("BiblioteQ: User Error"),
		 tr("It appears that you are attempting to decrease the "
		    "number of copies while there are copies "
		    "that have been reserved."));
	      ma.quantity->setValue(oldq);
	      return;
	    }
	  else if(newq > oldq)
	    if(QMessageBox::question
	       (this, tr("BiblioteQ: Question"),
		tr("Would you like to modify copy information?"),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No) == QMessageBox::Yes)
	      slotPopulateCopiesEditor();
	}

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().transaction())
	{
	  qapp->restoreOverrideCursor();
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to create a database transaction.")),
	     qmain->getDB().lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical
	    (this, tr("BiblioteQ: Database Error"),
	     tr("Unable to create a database transaction."));
	  return;
	}

      qapp->restoreOverrideCursor();
      str = ma.id->text().trimmed();
      ma.id->setText(str);

      if(ma.id->text().length() != 9)
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the ISSN field."));
	  ma.id->setFocus();
	  goto db_rollback;
	}

      str = ma.title->text().trimmed();
      ma.title->setText(str);

      if(ma.title->text().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Title field."));
	  ma.title->setFocus();
	  goto db_rollback;
	}

      str = ma.publisher->toPlainText().trimmed();
      ma.publisher->setPlainText(str);

      if(ma.publisher->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Publisher field."));
	  ma.publisher->setFocus();
	  goto db_rollback;
	}

      str = ma.place->toPlainText().trimmed();
      ma.place->setPlainText(str);

      if(ma.place->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Place of Publication "
				   "field."));
	  ma.place->setFocus();
	  goto db_rollback;
	}

      str = ma.category->toPlainText().trimmed();
      ma.category->setPlainText(str);

      if(ma.category->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Categories field."));
	  ma.category->setFocus();
	  goto db_rollback;
	}

      str = ma.description->toPlainText().trimmed();
      ma.description->setPlainText(str);

      if(ma.description->toPlainText().isEmpty())
	{
	  QMessageBox::critical(this, tr("BiblioteQ: User Error"),
				tr("Please complete the Abstract field."));
	  ma.description->setFocus();
	  goto db_rollback;
	}

      str = ma.lcnum->text().trimmed();
      ma.lcnum->setText(str);
      str = ma.callnum->text().trimmed();
      ma.callnum->setText(str);
      str = ma.deweynum->text().trimmed();
      ma.deweynum->setText(str);
      str = ma.url->toPlainText().trimmed();
      ma.url->setPlainText(str);

      if(engWindowTitle.contains("Modify"))
	query.prepare(QString("UPDATE %1 SET "
			      "id = ?, "
			      "title = ?, "
			      "pdate = ?, "
			      "publisher = ?, "
			      "category = ?, price = ?, "
			      "description = ?, "
			      "language = ?, "
			      "monetary_units = ?, "
			      "quantity = ?, "
			      "location = ?, "
			      "issuevolume = ?, "
			      "issueno = ?, "
			      "lccontrolnumber = ?, "
			      "callnumber = ?, "
			      "deweynumber = ?, "
			      "front_cover = ?, "
			      "back_cover = ?, "
			      "offsystem_url = ?, "
			      "place = ? "
			      "WHERE "
			      "myoid = ?").arg(subType));
      else if(qmain->getDB().driverName() != "QSQLITE")
	query.prepare(QString("INSERT INTO %1 "
			      "(id, "
			      "title, "
			      "pdate, publisher, "
			      "category, price, description, language, "
			      "monetary_units, quantity, "
			      "location, issuevolume, issueno, "
			      "lccontrolnumber, callnumber, deweynumber, "
			      "front_cover, back_cover, "
			      "offsystem_url, place, type) "
			      "VALUES (?, ?, "
			      "?, ?, "
			      "?, ?, ?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)").arg
		      (subType));
      else
	query.prepare(QString("INSERT INTO %1 "
			      "(id, "
			      "title, "
			      "pdate, publisher, "
			      "category, price, description, language, "
			      "monetary_units, quantity, "
			      "location, issuevolume, issueno, "
			      "lccontrolnumber, callnumber, deweynumber, "
			      "front_cover, back_cover, "
			      "offsystem_url, place, type, myoid) "
			      "VALUES (?, ?, ?, "
			      "?, ?, ?, "
			      "?, ?, "
			      "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)").arg
		      (subType));

      query.bindValue(0, ma.id->text());
      query.bindValue(1, ma.title->text());
      query.bindValue(2, ma.publication_date->date().toString("MM/dd/yyyy"));
      query.bindValue(3, ma.publisher->toPlainText());
      query.bindValue(4, ma.category->toPlainText().trimmed());
      query.bindValue(5, ma.price->text());
      query.bindValue(6, ma.description->toPlainText());
      query.bindValue(7, ma.language->currentText().trimmed());
      query.bindValue(8, ma.monetary_units->currentText().trimmed());
      query.bindValue(9, ma.quantity->text());
      query.bindValue(10, ma.location->currentText().trimmed());
      query.bindValue(11, ma.volume->text());
      query.bindValue(12, ma.issue->text());

      if(!ma.lcnum->text().isEmpty())
	query.bindValue(13, ma.lcnum->text());
      else
	query.bindValue(13, QVariant(QVariant::String));

      if(!ma.callnum->text().isEmpty())
	query.bindValue(14, ma.callnum->text());
      else
	query.bindValue(14, QVariant(QVariant::String));

      if(!ma.deweynum->text().isEmpty())
	query.bindValue(15, ma.deweynum->text());
      else
	query.bindValue(15, QVariant(QVariant::String));

      if(!ma.front_image->image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);
	  buffer.open(QIODevice::WriteOnly);
	  ma.front_image->image.save
	    (&buffer, ma.front_image->imageFormat.toAscii(), 100);
	  query.bindValue(16, bytes);
	}
      else
	{
	  ma.front_image->imageFormat = "";
	  query.bindValue(16, QVariant());
	}

      if(!ma.back_image->image.isNull())
	{
	  QByteArray bytes;
	  QBuffer buffer(&bytes);
	  buffer.open(QIODevice::WriteOnly);
	  ma.back_image->image.save
	    (&buffer, ma.back_image->imageFormat.toAscii(), 100);
	  query.bindValue(17, bytes);
	}
      else
	{
	  ma.back_image->imageFormat = "";
	  query.bindValue(17, QVariant());
	}

      if(!ma.url->toPlainText().isEmpty())
	query.bindValue(18, ma.url->toPlainText());
      else
	query.bindValue(18, QVariant(QVariant::String));

      query.bindValue(19, ma.place->toPlainText().trimmed());

      if(engWindowTitle.contains("Modify"))
	query.bindValue(20, oid);
      else
	query.bindValue(20, subType);

      if(engWindowTitle.contains("Create"))
	if(qmain->getDB().driverName() == "QSQLITE")
	  query.bindValue(21,
			  ma.id->text().remove("-") +
			  ma.volume->text() + ma.issue->text());

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!query.exec())
	{
	  qapp->restoreOverrideCursor();
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to create or update the entry.")),
	     query.lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical
	    (this, tr("BiblioteQ: Database Error"),
	     tr("Unable to create or update the entry. "
		"Please verify that "
		"the entry does not already exist."));
	  goto db_rollback;
	}
      else
	{
	  /*
	  ** Remove copies if the quantity has been decreased.
	  */

	  if(engWindowTitle.contains("Modify"))
	    {
	      query.prepare(QString("DELETE FROM %1_copy_info WHERE "
				    "copy_number > ? AND "
				    "item_oid = "
				    "?").arg(subType));
	      query.bindValue(0, ma.quantity->text());
	      query.bindValue(1, oid);

	      if(!query.exec())
		{
		  qapp->restoreOverrideCursor();
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to purge unnecessary copy "
				"data.")),
		     query.lastError().text(), __FILE__,
		     __LINE__);
		  QMessageBox::critical(this,
					tr("BiblioteQ: Database Error"),
					tr("Unable to purge unnecessary "
					   "copy data."));
		  goto db_rollback;
		}

	      if(!qmain->getDB().commit())
		{
		  qapp->restoreOverrideCursor();
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to commit the current database "
				"transaction.")),
		     qmain->getDB().lastError().text(), __FILE__, __LINE__);
		  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
					tr("Unable to commit the current "
					   "database transaction."));
		  goto db_rollback;
		}
	    }
	  else
	    {
	      /*
	      ** Create initial copies.
	      */

	      misc_functions::createInitialCopies
		(ma.id->text() + "," + ma.volume->text() + "," +
		 ma.issue->text(), ma.quantity->value(),
		 qmain->getDB(), subType, errorstr);

	      if(!errorstr.isEmpty())
		{
		  qapp->restoreOverrideCursor();
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to create initial copies.")),
		     errorstr, __FILE__, __LINE__);
		  QMessageBox::critical
		    (this,
		     tr("BiblioteQ: Database Error"),
		     tr("Unable to create initial copies."));
		  goto db_rollback;
		}

	      if(!qmain->getDB().commit())
		{
		  qapp->restoreOverrideCursor();
		  qmain->addError
		    (QString(tr("Database Error")),
		     QString(tr("Unable to commit the current database "
				"transaction.")),
		     qmain->getDB().lastError().text(), __FILE__,
		     __LINE__);
		  QMessageBox::critical(this, tr("BiblioteQ: Database Error"),
					tr("Unable to commit the current "
					   "database transaction."));
		  goto db_rollback;
		}
	    }

	  ma.id->setPalette(te_orig_pal);
	  ma.category->viewport()->setPalette(te_orig_pal);
	  ma.place->viewport()->setPalette(te_orig_pal);
	  ma.lcnum->setPalette(ma.url->viewport()->palette());
	  ma.callnum->setPalette(ma.url->viewport()->palette());
	  ma.deweynum->setPalette(ma.url->viewport()->palette());
	  ma.title->setPalette(te_orig_pal);
	  ma.publication_date->setStyleSheet(dt_orig_ss);
	  ma.description->viewport()->setPalette(te_orig_pal);
	  ma.publisher->viewport()->setPalette(te_orig_pal);
	  oldq = ma.quantity->value();

	  if(ma.front_image->image.isNull())
	    ma.front_image->imageFormat = "";

	  if(ma.back_image->image.isNull())
	    ma.back_image->imageFormat = "";

	  if(subType == "Journal")
	    ma.publisher->setText
	      (QString("<a href=\"journal_search?publisher?%1\">" +
		       ma.publisher->toPlainText() + "</a>").arg
	       (ma.publisher->toPlainText()));
	  else
	    ma.publisher->setText
	      (QString("<a href=\"magazine_search?publisher?%1\">" +
		       ma.publisher->toPlainText() + "</a>").arg
	       (ma.publisher->toPlainText()));

	  if(subType == "Journal")
	    ma.place->setMultipleLinks("journal_search", "place",
				       ma.place->toPlainText());
	  else
	    ma.place->setMultipleLinks("magazine_search", "place",
				       ma.place->toPlainText());

	  if(subType == "Journal")
	    ma.category->setMultipleLinks("journal_search", "category",
					  ma.category->toPlainText());
	  else
	    ma.category->setMultipleLinks("magazine_search", "category",
					  ma.category->toPlainText());

	  if(!ma.url->toPlainText().isEmpty())
	    ma.url->setText(QString("<a href=\"%1\">%1</a>").arg
			    (ma.url->toPlainText()));

	  qapp->restoreOverrideCursor();

	  if(engWindowTitle.contains("Modify"))
	    {
	      if(subType == "Journal")
		str = QString(tr("BiblioteQ: Modify Journal Entry ("));
	      else
		str = QString(tr("BiblioteQ: Modify Magazine Entry ("));

	      str += ma.id->text() + tr(")");
	      setWindowTitle(str);
	      engWindowTitle = "Modify";

	      if((qmain->getUI().typefilter->currentText() == tr("All") ||
		  qmain->getUI().typefilter->currentText() ==
		  tr("All Overdue") ||
		  qmain->getUI().typefilter->currentText() ==
		  tr("All Requested") ||
		  qmain->getUI().typefilter->currentText() ==
		  tr("All Reserved") ||
		  qmain->getUI().typefilter->currentText() == tr("Journals") ||
		  qmain->getUI().typefilter->currentText() ==
		  tr("Magazines")) &&
		 oid == misc_functions::getColumnString(qmain->getUI().table,
							row, "MYOID"))
		{
		  for(i = 0; i < qmain->getUI().table->columnCount(); i++)
		    {
		      column = qmain->getUI().table->horizontalHeaderItem(i);

		      if(column == 0)
			continue;

		      if(column->text() == tr("ISSN") ||
			 column->text() == tr("ID Number"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.id->text());
		      else if(column->text() == tr("Title"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.title->text());
		      else if(column->text() == tr("Publication Date"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.publication_date->date().toString("MM/dd/yyyy"));
		      else if(column->text() == tr("Publisher"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.publisher->toPlainText());
		      else if(column->text() == tr("Place of Publication"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.place->toPlainText());
		      else if(column->text() == tr("Categories"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.category->toPlainText().trimmed());
		      else if(column->text() == tr("Price"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.price->text());
		      else if(column->text() == tr("Language"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.language->currentText().trimmed());
		      else if(column->text() == tr("Monetary Units"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.monetary_units->currentText().trimmed());
		      else if(column->text() == tr("Quantity"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.quantity->text());
		      else if(column->text() == tr("Location"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.location->currentText().trimmed());
		      else if(column->text() == tr("Volume"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.volume->text());
		      else if(column->text() == tr("Issue"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.issue->text());
		      else if(column->text() == tr("LC Control Number"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.lcnum->text());
		      else if(column->text() == tr("Call Number"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.callnum->text());
		      else if(column->text() == tr("Dewey Number"))
			qmain->getUI().table->item(row, i)->setText
			  (ma.deweynum->text());
		      else if(column->text() == tr("Availability"))
			{
			  qmain->getUI().table->item(row, i)->setText
			    (misc_functions::getAvailability
			     (oid, qmain->getDB(), subType,
			      errorstr));

			  if(!errorstr.isEmpty())
			    qmain->addError
			      (QString(tr("Database Error")),
			       QString(tr("Retrieving availability.")),
			       errorstr, __FILE__, __LINE__);
			}
		    }

		  foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
		    textfield->setCursorPosition(0);

		  if(qmain->getUI().actionAutoResizeColumns->isChecked())
		    qmain->slotResizeColumns();

		  qmain->slotDisplaySummary();
		}
	    }
	  else
	    {
	      qapp->setOverrideCursor(Qt::WaitCursor);
	      oid = misc_functions::getOID(ma.id->text() + "," +
					   ma.volume->text() + "," +
					   ma.issue->text(),
					   subType,
					   qmain->getDB(),
					   errorstr);
	      qapp->restoreOverrideCursor();

	      if(!errorstr.isEmpty())
		{
		  oid = "insert";

		  if(subType == "Journal")
		    {
		      qmain->addError(QString(tr("Database Error")),
				      QString(tr("Unable to retrieve the "
						 "journal's OID.")),
				      errorstr, __FILE__, __LINE__);
		      QMessageBox::critical
			(this, tr("BiblioteQ: Database Error"),
			 QString(tr("Unable to retrieve the "
				    "journal's OID.")));
		    }
		  else
		    {
		      qmain->addError(QString(tr("Database Error")),
				      QString(tr("Unable to retrieve the "
						 "magazine's OID.")),
				      errorstr, __FILE__, __LINE__);
		      QMessageBox::critical
			(this, tr("BiblioteQ: Database Error"),
			 QString(tr("Unable to retrieve the "
				    "magazine's OID.")));
		    }
		}
	      else if(subType == "Journal")
		qmain->replaceJournal
		  ("insert", static_cast<qtbook_journal *> (this));
	      else if(subType == "Magazine")
		qmain->replaceMagazine("insert", this);

	      updateWindow(qtbook::EDITABLE);

	      if(qmain->getUI().actionAutoPopulateOnCreation->isChecked())
		{
		  if(subType == "Journal")
		    (void) qmain->populateTable
		      (qtbook::POPULATE_ALL, QString(tr("Journals")),
		       QString(""));
		  else
		    (void) qmain->populateTable
		      (qtbook::POPULATE_ALL, QString(tr("Magazines")),
		       QString(""));
		}

	      raise();
	    }

	  storeData(this);
	}

      return;

    db_rollback:

      qapp->setOverrideCursor(Qt::WaitCursor);

      if(!qmain->getDB().rollback())
	qmain->addError(QString(tr("Database Error")),
			QString(tr("Rollback failure.")),
			qmain->getDB().lastError().text(), __FILE__,
			__LINE__);

      qapp->restoreOverrideCursor();
    }
  else
    {
      searchstr = QString("SELECT %1.title, "
			  "%1.publisher, %1.pdate, %1.place, "
			  "%1.issuevolume, "
			  "%1.issueno, "
			  "%1.category, %1.language, "
			  "%1.id, "
			  "%1.price, %1.monetary_units, "
			  "%1.quantity, "
			  "%1.location, "
			  "%1.lccontrolnumber, "
			  "%1.callnumber, "
			  "%1.deweynumber, "
			  "%1.quantity - COUNT(item_borrower_vw."
			  "item_oid) "
			  "AS availability, "
			  "%1.type, "
			  "%1.myoid "
			  "FROM "
			  "%1 LEFT JOIN item_borrower_vw ON "
			  "%1.myoid = item_borrower_vw.item_oid "
			  "AND item_borrower_vw.type = '%1' "
			  "WHERE %1.type = '%1' AND ").arg(subType);
      searchstr.append("id LIKE '%" + ma.id->text() + "%' AND ");
      searchstr.append("LOWER(COALESCE(lccontrolnumber, '')) LIKE '%" +
		       myqstring::escape(ma.lcnum->text().toLower()) +
		       "%' AND ");
      searchstr.append("LOWER(COALESCE(callnumber, '')) LIKE '%" +
		       myqstring::escape(ma.callnum->text().toLower()) +
		       "%' AND ");
      searchstr.append("LOWER(COALESCE(deweynumber, '')) LIKE '%" +
		       myqstring::escape(ma.deweynum->text().toLower()) +
		       "%' AND ");

      if(ma.volume->value() != -1)
	searchstr.append("issuevolume = " + ma.volume->text() +
			 " AND ");

      if(ma.issue->value() != -1)
	searchstr.append("issueno = " + ma.issue->text() +
			 " AND ");

      searchstr.append("LOWER(title) LIKE '%" +
		       myqstring::escape(ma.title->text().toLower()) +
		       "%' AND ");

      if(ma.publication_date->date().toString
	 ("MM/yyyy") != "01/7999")
	searchstr.append("SUBSTR(pdate, 1, 3) || SUBSTR(pdate, 7) = '" +
			 ma.publication_date->date().toString
			 ("MM/yyyy") +
			 "' AND ");

      searchstr.append("LOWER(publisher) LIKE '%" +
		       myqstring::escape
		       (ma.publisher->toPlainText().toLower()) +
		       "%' AND ");
      searchstr.append("LOWER(place) LIKE '%" +
		       myqstring::escape
		       (ma.place->toPlainText().toLower()) +
		       "%' AND ");
      searchstr.append("LOWER(category) LIKE '%" +
		       myqstring::escape(ma.category->toPlainText().
					 toLower()) +
		       "%' AND ");

      if(ma.price->value() > -0.01)
	{
	  searchstr.append("price = ");
	  searchstr.append(ma.price->text());
	  searchstr.append(" AND ");
	}

      if(ma.language->currentText() != tr("Any"))
	searchstr.append("language = '" +
			 myqstring::escape(ma.language->currentText()) +
			 "' AND ");

      if(ma.monetary_units->currentText() != tr("Any"))
	searchstr.append("monetary_units = '" +
			 myqstring::escape
			 (ma.monetary_units->currentText()) +
			 "' AND ");

      searchstr.append("LOWER(description) LIKE '%" +
		       myqstring::escape
		       (ma.description->toPlainText().toLower()) + "%' ");

      if(ma.quantity->value() != 0)
	searchstr.append("AND quantity = " + ma.quantity->text() + " ");

      if(ma.location->currentText() != tr("Any"))
	searchstr.append("AND location = '" +
			 myqstring::escape
			 (ma.location->currentText()) + "' ");

      if(!ma.url->toPlainText().isEmpty())
	searchstr.append(" AND LOWER(COALESCE(offsystem_url, '')) LIKE '%" +
			 myqstring::escape
			 (ma.url->toPlainText().toLower()) + "%' ");

      hide();

      /*
      ** Search the database.
      */

      if(subType == "Journal")
	(void) qmain->populateTable
	  (qtbook::POPULATE_SEARCH, QString(tr("Journals")), searchstr);
      else
	(void) qmain->populateTable
	  (qtbook::POPULATE_SEARCH, QString(tr("Magazines")), searchstr);

      if(subType == "Journal")
	/*
	** Call qtbook_journal's closeEvent() method.
	*/

	close();
      else
	/*
	** Call qtbook_magazine's closeEvent() method.
	*/

	slotCancel();
    }
}

/*
** -- search() --
*/

void qtbook_magazine::search(const QString &field, const QString &value)
{
  ma.coverImages->setVisible(false);
  ma.id->clear();
  ma.lcnum->clear();
  ma.callnum->clear();
  ma.deweynum->clear();
  ma.title->clear();
  ma.publisher->clear();
  ma.place->clear();
  ma.description->clear();
  ma.category->clear();
  ma.copiesButton->setVisible(false);
  ma.showUserButton->setVisible(false);
  ma.queryButton->setVisible(false);
  ma.okButton->setText(tr("&Search"));
  ma.publication_date->setDate(QDate::fromString("01/7999",
						 "MM/yyyy"));
  ma.publication_date->setDisplayFormat("MM/yyyy");
  ma.id->setCursorPosition(0);
  ma.price->setMinimum(-0.01);
  ma.price->setValue(-0.01);
  ma.quantity->setMinimum(0);
  ma.quantity->setValue(0);
  ma.volume->setMinimum(-1);
  ma.volume->setValue(-1);
  ma.issue->setMinimum(-1);
  ma.issue->setValue(-1);

  /*
  ** Add "any".
  */

  if(ma.language->findText(tr("Any")) == -1)
    ma.language->insertItem(0, tr("Any"));

  if(ma.monetary_units->findText(tr("Any")) == -1)
    ma.monetary_units->insertItem(0, tr("Any"));

  if(ma.location->findText(tr("Any")) == -1)
    ma.location->insertItem(0, tr("Any"));

  ma.location->setCurrentIndex(0);
  ma.language->setCurrentIndex(0);
  ma.monetary_units->setCurrentIndex(0);
  ma.url->clear();

  if(field.isEmpty() && value.isEmpty())
    {
      QList<QAction *> actions = ma.resetButton->menu()->actions();
      
      actions[0]->setVisible(false);
      actions[1]->setVisible(false);
      actions.clear();

      if(subType == "Journal")
	setWindowTitle(QString(tr("BiblioteQ: Database Journal Search")));
      else
	setWindowTitle(QString(tr("BiblioteQ: Database Magazine Search")));

      engWindowTitle = "Search";
      ma.id->setFocus();
      misc_functions::center(this, parentWid);
      show();
    }
  else
    {
      if(field == "publisher")
	ma.publisher->setPlainText(value);
      else if(field == "category")
	ma.category->setPlainText(value);
      else if(field == "place")
	ma.place->setPlainText(value);

      slotGo();
    }
}

/*
** -- updateWindow() --
*/

void qtbook_magazine::updateWindow(const int state)
{
  QString str = "";

  if(state == qtbook::EDITABLE)
    {
      ma.showUserButton->setEnabled(true);
      ma.copiesButton->setEnabled(true);
      ma.queryButton->setVisible(true);
      ma.okButton->setVisible(true);
      ma.resetButton->setVisible(true);
      ma.frontButton->setVisible(true);
      ma.backButton->setVisible(true);

      if(subType == "Journal")
	str = QString(tr("BiblioteQ: Modify Journal Entry ("));
      else
	str = QString(tr("BiblioteQ: Modify Magazine Entry ("));

      str += ma.id->text() + tr(")");
      engWindowTitle = "Modify";
    }
  else
    {
      ma.showUserButton->setEnabled(true);
      ma.copiesButton->setVisible(false);
      ma.queryButton->setVisible(false);
      ma.okButton->setVisible(false);
      ma.resetButton->setVisible(false);
      ma.frontButton->setVisible(false);
      ma.backButton->setVisible(false);

      if(subType == "Journal")
	str = QString(tr("BiblioteQ: View Journal Details ("));
      else
	str = QString(tr("BiblioteQ: View Magazine Details ("));

      str += ma.id->text() + tr(")");
      engWindowTitle = "View";
    }

  ma.coverImages->setVisible(true);
  setWindowTitle(str);
}

/*
** -- modify() --
*/

void qtbook_magazine::modify(const int state)
{
  int i = 0;
  QString str = "";
  QString fieldname = "";
  QString searchstr = "";
  QVariant var;
  QSqlQuery query(qmain->getDB());

  if(state == qtbook::EDITABLE)
    {
      if(subType == "Journal")
	setWindowTitle(QString(tr("BiblioteQ: Modify Journal Entry")));
      else
	setWindowTitle(QString(tr("BiblioteQ: Modify Magazine Entry")));

      engWindowTitle = "Modify";
      ma.showUserButton->setEnabled(true);
      ma.copiesButton->setEnabled(true);
      ma.queryButton->setVisible(true);
      ma.okButton->setVisible(true);
      ma.resetButton->setVisible(true);
      ma.frontButton->setVisible(true);
      ma.backButton->setVisible(true);
      misc_functions::highlightWidget
	(ma.id, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(ma.title, QColor(255, 248, 220));
      misc_functions::highlightWidget
	(ma.publisher->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(ma.description->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(ma.category->viewport(), QColor(255, 248, 220));
      misc_functions::highlightWidget
	(ma.place->viewport(), QColor(255, 248, 220));
      te_orig_pal = ma.description->viewport()->palette();
    }
  else
    {
      if(subType == "Journal")
	setWindowTitle(QString(tr("BiblioteQ: View Journal Details")));
      else
	setWindowTitle(QString(tr("BiblioteQ: View Magazine Details")));

      engWindowTitle = "Modify";
      ma.showUserButton->setEnabled(true);
      ma.copiesButton->setVisible(false);
      ma.queryButton->setVisible(false);
      ma.okButton->setVisible(false);
      ma.resetButton->setVisible(false);
      ma.frontButton->setVisible(false);
      ma.backButton->setVisible(false);

      QList<QAction *> actions = ma.resetButton->menu()->actions();
      
      actions[0]->setVisible(false);
      actions[1]->setVisible(false);
      actions.clear();
    }

  ma.quantity->setMinimum(1);
  ma.price->setMinimum(0.00);
  ma.okButton->setText(tr("&Save"));
  ma.volume->setMinimum(0);
  ma.issue->setMinimum(0);
  str = oid;
  searchstr = QString("SELECT title, "
		      "publisher, pdate, place, issuevolume, "
		      "category, language, id, "
		      "price, monetary_units, quantity, "
		      "issueno, "
		      "location, lccontrolnumber, callnumber, "
		      "deweynumber, description, "
		      "front_cover, "
		      "back_cover, "
		      "offsystem_url "
		      "FROM "
		      "%1 "
		      "WHERE myoid = ").arg(subType);
  searchstr.append(str);
  qapp->setOverrideCursor(Qt::WaitCursor);

  if(!query.exec(searchstr) || !query.next())
    {
      qapp->restoreOverrideCursor();

      if(subType == "Journal")
	{
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to retrieve the selected journal's data.")),
	     query.lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical
	    (this, tr("BiblioteQ: Database Error"),
	     QString(tr("Unable to retrieve the selected journal's "
			"data.")));
	}
      else
	{
	  qmain->addError
	    (QString(tr("Database Error")),
	     QString(tr("Unable to retrieve the selected magazine's data.")),
	     query.lastError().text(), __FILE__, __LINE__);
	  QMessageBox::critical
	    (this, tr("BiblioteQ: Database Error"),
	     QString(tr("Unable to retrieve the selected magazine's "
			"data.")));
	}

      return;
    }
  else
    {
      qapp->restoreOverrideCursor();
      showNormal();

      for(i = 0; i < query.record().count(); i++)
	{
	  var = query.record().field(i).value();
	  fieldname = query.record().fieldName(i);

	  if(fieldname == "title")
	    ma.title->setText(var.toString());
	  else if(fieldname == "publisher")
	    {
	      if(subType == "Journal")
		ma.publisher->setText
		  (QString("<a href=\"journal_search?publisher?%1\">" +
			   var.toString() + "</a>").arg(var.toString()));
	      else
		ma.publisher->setText
		  (QString("<a href=\"magazine_search?publisher?%1\">" +
			   var.toString() + "</a>").arg(var.toString()));
	    }
	  else if(fieldname == "pdate")
	    ma.publication_date->setDate
	      (QDate::fromString(var.toString(), "MM/dd/yyyy"));
	  else if(fieldname == "price")
	    ma.price->setValue(var.toDouble());
	  else if(fieldname == "place")
	    {
	      if(subType == "Journal")
		ma.place->setMultipleLinks("journal_search", "place",
					   var.toString());
	      else
		ma.place->setMultipleLinks("magazine_search", "place",
					   var.toString());
	    }
	  else if(fieldname == "category")
	    {
	      if(subType == "Journal")
		ma.category->setMultipleLinks("journal_search", "category",
					      var.toString());
	      else
		ma.category->setMultipleLinks("magazine_search", "category",
					      var.toString());
	    }
	  else if(fieldname == "language")
	    {
	      if(ma.language->findText(var.toString()) > -1)
		ma.language->setCurrentIndex
		  (ma.language->findText(var.toString()));
	      else
		ma.language->setCurrentIndex
		  (ma.language->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "quantity")
	    ma.quantity->setValue(var.toInt());
	  else if(fieldname == "monetary_units")
	    {
	      if(ma.monetary_units->findText(var.toString()) > -1)
		ma.monetary_units->setCurrentIndex
		  (ma.monetary_units->findText(var.toString()));
	      else
		ma.monetary_units->setCurrentIndex
		  (ma.monetary_units->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "issuevolume")
	    ma.volume->setValue(var.toInt());
	  else if(fieldname == "issueno")
	    ma.issue->setValue(var.toInt());
	  else if(fieldname == "location")
	    {
	      if(ma.location->findText(var.toString()) > -1)
		ma.location->setCurrentIndex
		  (ma.location->findText(var.toString()));
	      else
		ma.location->setCurrentIndex
		  (ma.location->findText(tr("UNKNOWN")));
	    }
	  else if(fieldname == "id")
	    {
	      if(state == qtbook::EDITABLE)
		{
		  if(subType == "Journal")
		    str = QString(tr("BiblioteQ: Modify Journal Entry (")) +
		      var.toString() + tr(")");
		  else
		    str = QString(tr("BiblioteQ: Modify Magazine Entry (")) +
		      var.toString() + tr(")");

		  engWindowTitle = "Modify";
		}
	      else
		{
		  if(subType == "Journal")
		    str = QString(tr("BiblioteQ: View Journal Details (")) +
		      var.toString() + tr(")");
		  else
		    str = QString(tr("BiblioteQ: View Magazine Details (")) +
		      var.toString() + tr(")");

		  engWindowTitle = "View";
		}

	      setWindowTitle(str);
	      ma.id->setText(var.toString());
	    }
	  else if(fieldname == "description")
	    ma.description->setPlainText(var.toString());
	  else if(fieldname == "lccontrolnumber")
	    ma.lcnum->setText(var.toString());
	  else if(fieldname == "callnumber")
	    ma.callnum->setText(var.toString());
	  else if(fieldname == "deweynumber")
	    ma.deweynum->setText(var.toString());
	  else if(fieldname == "front_cover")
	    {
	      if(!query.record().field(i).isNull())
		ma.front_image->loadFromData(var.toByteArray());
	    }
	  else if(fieldname == "back_cover")
	    {
	      if(!query.record().field(i).isNull())
		ma.back_image->loadFromData(var.toByteArray());
	    }
	  else if(fieldname == "offsystem_url")
	    {
	      if(!query.record().field(i).isNull())
		ma.url->setText(QString("<a href=\"%1\">%1</a>").arg
				(var.toString()));
	    }
	}

      foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
	textfield->setCursorPosition(0);

      storeData(this);
    }

  ma.id->setFocus();
  raise();
}

/*
** -- insert() --
*/

void qtbook_magazine::insert(void)
{
  slotReset();
  ma.id->clear();
  ma.lcnum->clear();
  ma.callnum->clear();
  ma.deweynum->clear();
  ma.title->clear();
  ma.publisher->setPlainText("N/A");
  ma.description->setPlainText("N/A");
  ma.category->setPlainText("N/A");
  ma.place->setPlainText("N/A");
  ma.copiesButton->setEnabled(false);
  ma.queryButton->setVisible(true);
  ma.okButton->setText(tr("&Save"));
  ma.publication_date->setDate(QDate::fromString("01/01/2000",
						 "MM/dd/yyyy"));
  ma.id->setCursorPosition(0);
  ma.price->setMinimum(0.00);
  ma.price->setValue(0.00);
  ma.quantity->setMinimum(1);
  ma.quantity->setValue(1);
  ma.volume->setMinimum(0);
  ma.volume->setValue(0);
  ma.issue->setMinimum(0);
  ma.issue->setValue(0);
  ma.showUserButton->setEnabled(false);
  ma.location->setCurrentIndex(0);
  ma.language->setCurrentIndex(0);
  ma.monetary_units->setCurrentIndex(0);
  ma.url->clear();
  misc_functions::highlightWidget
    (ma.id, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (ma.title, QColor(255, 248, 220));
  misc_functions::highlightWidget
    (ma.publisher->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (ma.description->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (ma.category->viewport(), QColor(255, 248, 220));
  misc_functions::highlightWidget
    (ma.place->viewport(), QColor(255, 248, 220));
  te_orig_pal = ma.description->viewport()->palette();

  if(subType == "Journal")
    setWindowTitle(QString(tr("BiblioteQ: Create Journal Entry")));
  else
    setWindowTitle(QString(tr("BiblioteQ: Create Magazine Entry")));

  engWindowTitle = "Create";
  ma.id->setFocus();
  storeData(this);
  misc_functions::center(this, parentWid);
  show();
}

/*
** -- slotReset() --
*/

void qtbook_magazine::slotReset(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(action != 0)
    {
      QList<QAction *> actions = ma.resetButton->menu()->actions();

      if(action == actions[0])
	ma.front_image->clear();
      else if(action == actions[1])
	ma.back_image->clear();
      else if(action == actions[2])
	{
	  ma.id->clear();
	  ma.id->setCursorPosition(0);
	  ma.id->setPalette(te_orig_pal);
	  ma.id->setFocus();
	}
      else if(action == actions[8])
	{
	  ma.title->clear();
	  ma.title->setPalette(te_orig_pal);
	  ma.title->setFocus();
	}
      else if(action == actions[3])
	{
	  ma.volume->setValue(ma.volume->minimum());
	  ma.volume->setFocus();
	}
      else if(action == actions[4])
	{
	  ma.issue->setValue(ma.issue->minimum());
	  ma.issue->setFocus();
	}
      else if(action == actions[9])
	{
	  if(engWindowTitle.contains("Search"))
	    ma.publication_date->setDate
	      (QDate::fromString("01/7999", "MM/yyyy"));
	  else
	    ma.publication_date->setDate
	      (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

	  ma.publication_date->setStyleSheet(dt_orig_ss);
	  ma.publication_date->setFocus();
	}
      else if(action == actions[10])
	{
	  if(!engWindowTitle.contains("Search"))
	    ma.publisher->setPlainText("N/A");
	  else
	    ma.publisher->clear();

	  ma.publisher->viewport()->setPalette(te_orig_pal);
	  ma.publisher->setFocus();
	}
      else if(action == actions[11])
	{
	  if(!engWindowTitle.contains("Search"))
	    ma.place->setPlainText("N/A");
	  else
	    ma.place->clear();

	  ma.place->viewport()->setPalette(te_orig_pal);
	  ma.place->setFocus();
	}
      else if(action == actions[12])
	{
	  if(!engWindowTitle.contains("Search"))
	    ma.category->setPlainText("N/A");
	  else
	    ma.category->clear();

	  ma.category->viewport()->setPalette(te_orig_pal);
	  ma.category->setFocus();
	}
      else if(action == actions[13])
	{
	  ma.price->setValue(ma.price->minimum());
	  ma.price->setFocus();
	}
      else if(action == actions[14])
	{
	  ma.language->setCurrentIndex(0);
	  ma.language->setFocus();
	}
      else if(action == actions[15])
	{
	  ma.monetary_units->setCurrentIndex(0);
	  ma.monetary_units->setFocus();
	}
      else if(action == actions[18])
	{
	  if(!engWindowTitle.contains("Search"))
	    ma.description->setPlainText("N/A");
	  else
	    ma.description->clear();

	  ma.description->viewport()->setPalette(te_orig_pal);
	  ma.description->setFocus();
	}
      else if(action == actions[16])
	{
	  ma.quantity->setValue(ma.quantity->minimum());
	  ma.quantity->setFocus();
	}
      else if(action == actions[17])
	{
	  ma.location->setCurrentIndex(0);
	  ma.location->setFocus();
	}
      else if(action == actions[5])
	{
	  ma.lcnum->clear();
	  ma.lcnum->setPalette(ma.url->viewport()->palette());
	  ma.lcnum->setFocus();
	}
      else if(action == actions[6])
	{
	  ma.callnum->clear();
	  ma.callnum->setPalette(ma.url->viewport()->palette());
	  ma.callnum->setFocus();
	}
      else if(action == actions[7])
	{
	  ma.deweynum->clear();
	  ma.deweynum->setPalette(ma.url->viewport()->palette());
	  ma.deweynum->setFocus();
	}
      else if(action == actions[19])
	{
	  ma.url->clear();
	  ma.url->setFocus();
	}

      actions.clear();
    }
  else
    {
      /*
      ** Reset all.
      */

      ma.id->clear();
      ma.id->setCursorPosition(0);
      ma.title->clear();

      if(!engWindowTitle.contains("Search"))
	ma.category->setPlainText("N/A");
      else
	ma.category->clear();

      if(!engWindowTitle.contains("Search"))
	ma.place->setPlainText("N/A");
      else
	ma.place->clear();

      if(!engWindowTitle.contains("Search"))
	ma.publisher->setPlainText("N/A");
      else
	ma.publisher->clear();

      if(!engWindowTitle.contains("Search"))
	ma.description->setPlainText("N/A");
      else
	ma.description->clear();

      ma.volume->setValue(ma.volume->minimum());
      ma.issue->setValue(ma.issue->minimum());
      ma.price->setValue(ma.price->minimum());

      if(engWindowTitle.contains("Search"))
	ma.publication_date->setDate
	  (QDate::fromString("01/7999", "MM/yyyy"));
      else
	ma.publication_date->setDate
	  (QDate::fromString("01/01/2000", "MM/dd/yyyy"));

      ma.language->setCurrentIndex(0);
      ma.monetary_units->setCurrentIndex(0);
      ma.quantity->setValue(ma.quantity->minimum());
      ma.location->setCurrentIndex(0);
      ma.lcnum->clear();
      ma.callnum->clear();
      ma.deweynum->clear();
      ma.front_image->clear();
      ma.back_image->clear();
      ma.url->clear();
      ma.id->setPalette(te_orig_pal);
      ma.category->viewport()->setPalette(te_orig_pal);
      ma.place->viewport()->setPalette(te_orig_pal);
      ma.lcnum->setPalette(ma.url->viewport()->palette());
      ma.callnum->setPalette(ma.url->viewport()->palette());
      ma.deweynum->setPalette(ma.url->viewport()->palette());
      ma.title->setPalette(te_orig_pal);
      ma.publication_date->setStyleSheet(dt_orig_ss);
      ma.description->viewport()->setPalette(te_orig_pal);
      ma.publisher->viewport()->setPalette(te_orig_pal);
      ma.id->setFocus();
    }
}

/*
** -- closeEvent() --
*/

void qtbook_magazine::closeEvent(QCloseEvent *e)
{
  if(engWindowTitle.contains("Create") ||
     engWindowTitle.contains("Modify"))
    if(hasDataChanged(this))
      if(QMessageBox::question(this, tr("BiblioteQ: Question"),
			       tr("You have unsaved data. Continue closing?"),
			       QMessageBox::Yes | QMessageBox::No,
			       QMessageBox::No) == QMessageBox::No)
	{
	  e->ignore();
	  return;
	}

  qmain->removeMagazine(this);
}

/*
** -- slotCancel() --
*/

void qtbook_magazine::slotCancel(void)
{
  close();
}

/*
** -- slotPopulateCopiesEditor() --
*/

void qtbook_magazine::slotPopulateCopiesEditor(void)
{
  copy_editor *copyeditor = 0;

  if((copyeditor = new(std::nothrow) copy_editor
      (static_cast<QWidget *> (this),
       static_cast<qtbook_item *> (this),
       false,
       ma.quantity->value(), oid,
       ma.id->text(),
       ma.quantity, font(), subType)) != 0)
    copyeditor->populateCopiesEditor();
}

/*
** -- slotShowUsers() --
*/

void qtbook_magazine::slotShowUsers(void)
{
  int state = 0;
  borrowers_editor *borrowerseditor = 0;

  if(!ma.okButton->isHidden())
    state = qtbook::EDITABLE;
  else
    state = qtbook::VIEW_ONLY;

  if((borrowerseditor = new(std::nothrow) borrowers_editor
      (static_cast<QWidget *> (this), static_cast<qtbook_item *> (this),
       ma.quantity->value(), oid, ma.id->text(), font(), subType,
       state)) != 0)
    borrowerseditor->showUsers();
}

/*
** -- slotQuery() --
*/

void qtbook_magazine::slotQuery(void)
{
  int i = 0;
  QString etype = "";
  QString errorstr = "";
  QString searchstr = "";
  locresults *dialog = 0;
  QStringList list;
  qtbook_item_working_dialog working(static_cast<QMainWindow *> (this));

  if(ma.id->text().trimmed().length() != 9)
    {
      QMessageBox::critical
	(this, tr("BiblioteQ: User Error"),
	 tr("In order to query a Z39.50 system, the ISSN "
	    "must be provided."));
      ma.id->setFocus();
      return;
    }

  if((thread = new(std::nothrow) generic_thread()) != 0)
    {
      working.setModal(true);
      working.setWindowTitle(tr("BiblioteQ: Z39.50 Data Retrieval"));
      working.setLabelText(tr("Downloading information from the Z39.50 "
			      "system. Please be patient."));
      working.setMaximum(0);
      working.setMinimum(0);
      working.setCancelButton(0);
      working.show();
      working.update();
      searchstr = QString("@attr 1=8 %1").arg(ma.id->text());
      thread->setType(generic_thread::QUERY_LIBRARY_OF_CONGRESS);
      thread->setLOCSearchString(searchstr);
      thread->start();

      while(thread->isRunning())
	{
	  qapp->processEvents();
	  thread->msleep(10);
	}

      working.hide();

      if(working.wasCanceled())
	{
	  thread->deleteLater();
	  return;
	}

      if((errorstr = thread->getErrorStr()).isEmpty())
	{
	  if(thread->getLOCResults().size() == 1)
	    {
	      if(QMessageBox::question
		 (this, tr("BiblioteQ: Question"),
		  tr("Replace existing values with those retrieved "
		     "from the Z39.50 system?"),
		  QMessageBox::Yes | QMessageBox::No,
		  QMessageBox::No) == QMessageBox::Yes)
		{
		  list = QString(thread->getLOCResults()[0]).split("\n");
		  populateDisplayAfterLOC(list);
		  list.clear();
		}
	    }
	  else if(thread->getLOCResults().size() > 1)
	    {
	      for(i = 0; i < thread->getLOCResults().size(); i++)
		list.append(thread->getLOCResults()[i]);

	      /*
	      ** Display a selection dialog.
	      */

	      if((dialog = new(std::nothrow) locresults
		  (static_cast<QWidget *> (this), list,
		   this, font())) == 0)
		{
		  qmain->addError
		    (QString(tr("Memory Error")),
		     QString(tr("Unable to create a \"dialog\" object "
				"due to insufficient resources.")),
		     QString(""),
		     __FILE__, __LINE__);
		  QMessageBox::critical
		    (this, tr("BiblioteQ: Memory Error"),
		     tr("Unable to create a \"dialog\" object "
			"due to insufficient resources."));
		}
	    }
	  else
	    QMessageBox::critical
	      (this, tr("BiblioteQ: Z39.50 Query Error"),
	       tr("A Z39.50 entry may not yet exist for ") +
	       ma.id->text() + tr("."));
	}
      else
	etype = thread->getEType();

      thread->deleteLater();
    }
  else
    {
      etype = tr("Memory Error");
      errorstr = tr("Unable to create a thread due to insufficient "
		    "resources.");
    }

  if(!errorstr.isEmpty())
    {
      qmain->addError(QString(tr("Z39.50 Query Error")), etype, errorstr,
		      __FILE__, __LINE__);
      QMessageBox::critical
	(this, tr("BiblioteQ: Z39.50 Query Error"),
	 tr("The Z39.50 entry could not be retrieved."));
    }
}

/*
** -- slotPrint() --
*/

void qtbook_magazine::slotPrint(void)
{
  html = "";
  html += "<b>" + tr("ISSN:") + "</b> " + ma.id->text().trimmed() + "<br>";
  html += "<b>" + tr("Volume:") + "</b> " + ma.volume->text() + "<br>";
  html += "<b>" + tr("Issue (Number):") + "</b> " + ma.issue->text() + "<br>";
  html += "<b>" + tr("LC Control Number:") + "</b> " +
    ma.lcnum->text().trimmed() + "<br>";
  html += "<b>" + tr("Call Number:") + "</b> " +
    ma.callnum->text().trimmed() + "<br>";
  html += "<b>" + tr("Dewey Class Number:") + "</b> " +
    ma.deweynum->text().trimmed() + "<br>";

  /*
  ** General information.
  */

  html += "<b>" + tr("Title:") + "</b> " + ma.title->text().trimmed() + "<br>";
  html += "<b>" + tr("Publication Date:") + "</b> " +
    ma.publication_date->date().toString("MM/dd/yyyy") + "<br>";
  html += "<b>" + tr("Publisher:") + "</b> " +
    ma.publisher->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Place of Publication:") + "</b> " +
    ma.place->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Category:") + "</b> " +
    ma.category->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("Price:") + "</b> " + ma.price->text() + "<br>";
  html += "<b>" + tr("Language:") + "</b> " +
    ma.language->currentText() + "<br>";
  html += "<b>" + tr("Monetary Units:") + "</b> " +
    ma.monetary_units->currentText() + "<br>";
  html += "<b>" + tr("Copies:") + "</b> " + ma.quantity->text() + "<br>";
  html += "<b>" + tr("Location:") + "</b> " +
    ma.location->currentText() + "<br>";
  html += "<b>" + tr("Abstract:") + "</b> " +
    ma.description->toPlainText().trimmed() + "<br>";
  html += "<b>" + tr("OFFSYSTEM URL:") + "</b>" +
    ma.url->toPlainText().trimmed();
  print(this);
}

/*
** -- populateDisplayAfterLOC() --
*/

void qtbook_magazine::populateDisplayAfterLOC(const QStringList &list)
{
  int i = 0;
  int j = 0;
  QString str = "";
  QStringList tmplist;

  for(i = 0; i < list.size(); i++)
    if(list[i].startsWith("260"))
      ma.place->clear();
    else if(list[i].startsWith("650"))
      ma.category->clear();

  for(i = 0; i < list.size(); i++)
    {
      str = list[i];

      if(str.startsWith("010"))
	{
	  /*
	  ** $a - LC Control Number
	  ** $b - NUCMC Control Number
	  ** $z - Canceled/Invalid LC Control Number
	  ** $8 - Field Link and Sequence Number
	  */

	  str = str.mid(str.indexOf("$a") + 2).trimmed();
	  ma.lcnum->setText(str);
	  misc_functions::highlightWidget
	    (ma.lcnum, QColor(162, 205, 90));
	}
      else if(str.startsWith("050"))
	{
	  /*
	  ** $a - Classification Number
	  ** $b - Item Number
	  ** $d - Supplementary Class Number (Obsolete)
	  ** $3 - Materials Specified
	  ** $6 - Linkage
	  ** $8 - Field Link and Sequence Number
	  */

	  str = str.mid(str.indexOf("$a") + 2).trimmed();
	  str = str.remove(" $b").remove(" $d").remove(" $3").
	    remove(" $6").remove(" $8").trimmed();
	  ma.callnum->setText(str);
	  misc_functions::highlightWidget
	    (ma.callnum, QColor(162, 205, 90));
	}
      else if(str.startsWith("082"))
	{
	  /*
	  ** $a - Classification Number
	  ** $b - Item Number
	  ** $b - DDC Number--abridged NST Version (Obsolete)
	  ** $2 - Edition Number
	  ** $6 - Linkage
	  ** $8 - Field Link and Sequence Number
	  */

	  str = str.mid(str.indexOf("$a") + 2).trimmed();
	  str = str.remove(" $2").trimmed();
	  ma.deweynum->setText(str);
	  misc_functions::highlightWidget
	    (ma.deweynum, QColor(162, 205, 90));
	}
      else if(str.startsWith("245"))
	{
	  /*
	  ** $a - Title
	  ** $b - Remainder of Title
	  ** $c - Statement of Responsibility
	  ** $d - Designation of section (Obsolete)
	  ** $e - Name of Part/Section
	  ** $f - Inclusive Dates
	  ** $g - Bulk Dates
	  ** $h - Medium
	  ** $k - Form
	  ** $n - Number of Part/Section of a Work
	  ** $p - Name of Part/Section of a Work
	  ** $s - Version (NR)
	  ** $6 - Linkage (NR)
	  ** $8 - Field Link and Sequence Number
	  */

	  str = str.mid(str.indexOf("$a") + 2,
			str.indexOf("/") - str.indexOf("$a") - 2).trimmed();

	  /*
	  ** Let's perform some additional massaging.
	  */

	  if(str.contains(" : "))
	    str.replace(" : ", ": ");
	  else if(str.contains(" ; "))
	    str.replace(" ; ", "; ");

	  tmplist = str.split(" ");
	  str = "";

	  for(j = 0; j < tmplist.size(); j++)
	    if(j == 0 || j == tmplist.size() - 1)
	      str += tmplist[j].mid(0, 1).toUpper() +
		tmplist[j].mid(1) + " ";
	    else if(tmplist[j] == "a" || tmplist[j] == "an" ||
		    tmplist[j] == "and" || tmplist[j] == "but" ||
		    tmplist[j] == "of" || tmplist[j] == "or" ||
		    tmplist[j] == "the")
	      {
		if(tmplist[j - 1].contains(":"))
		  str += tmplist[j].mid(0, 1).toUpper() +
		    tmplist[j].mid(1) + " ";
		else
		  str += tmplist[j] + " ";
	      }
	    else
	      str += tmplist[j].mid(0, 1).toUpper() + tmplist[j].mid(1) + " ";

	  tmplist.clear();
	  str = str.trimmed();

	  if(!str.isEmpty() && str[str.length() - 1].isPunct())
	    str.remove(str.length() - 1, 1);

	  ma.title->setText(str.trimmed());
	  misc_functions::highlightWidget
	    (ma.title, QColor(162, 205, 90));
	}
      else if(str.startsWith("260"))
	{
	  /*
	  ** $a - Place of Publication, Distribution, etc.
	  ** $b - Name of Publisher, Distributor, etc.
	  ** $c - Date of Publication, Distribution, etc.
	  ** $d - Plate or Publisher's Number for Music
	  ** $e - Place of Manufacture
	  ** $f - Manufacturer
	  ** $g - Date of Manufacture
	  ** $3 - Materials Specified
	  ** $6 - Linkage
	  ** $8 - Field Link and Sequence Number
	  */

	  QString tmpstr = str.mid(str.indexOf("$a") + 2).
	    trimmed();

	  if(tmpstr.contains("$b"))
	    tmpstr = tmpstr.mid(0, tmpstr.indexOf("$b")).trimmed();
	  else
	    tmpstr = tmpstr.mid(0, tmpstr.indexOf("$c")).trimmed();

	  tmplist = tmpstr.split("$a");

	  for(j = 0; j < tmplist.size(); j++)
	    {
	      tmpstr = tmplist.at(j).trimmed();
	      tmpstr = tmpstr.mid(0, tmpstr.lastIndexOf(" ")).
		trimmed();

	      if(tmpstr.isEmpty())
		continue;

	      if(!tmpstr[0].isLetterOrNumber())
		tmpstr = tmpstr.mid(1).trimmed();

	      if(tmpstr.isEmpty())
		continue;

	      if(tmpstr[tmpstr.length() - 1] == ',')
		tmpstr.remove(tmpstr.length() - 1, 1);

	      if(ma.place->toPlainText().isEmpty())
		ma.place->setPlainText(tmpstr);
	      else
		ma.place->setPlainText(ma.place->toPlainText() +
				       "\n" + tmpstr);
	    }

	  misc_functions::highlightWidget
	    (ma.place->viewport(), QColor(162, 205, 90));
	  ma.publication_date->setDate
	    (QDate::fromString("01/01/" +
			       str.mid(str.trimmed().length() - 5, 4),
			       "MM/dd/yyyy"));
	  ma.publication_date->setStyleSheet
	    ("background-color: rgb(162, 205, 90)");
	  str = str.mid(str.indexOf("$b") + 2).trimmed();
	  str = str.mid(0, str.indexOf(",")).trimmed();
	  ma.publisher->setPlainText(str);
	  misc_functions::highlightWidget
	    (ma.publisher->viewport(), QColor(162, 205, 90));
	}
      else if(str.startsWith("300"))
	{
	  /*
	  ** $a - Extent
	  ** $b - Other Physical Details
	  ** $c - Dimensions
	  ** $e - Accompanying Material
	  ** $f - Type of Unit
	  ** $g - Size of Unit
	  ** $3 - Materials Specified
	  ** $6 - Linkage
	  ** $8 - Field Link and Sequence Number
	  */

	  str = str.mid(str.indexOf("$a") + 2).trimmed();
	  str = str.remove(" $b").trimmed();
	  str = str.remove(" $c").trimmed();
	  ma.description->setPlainText(str);
	  misc_functions::highlightWidget
	    (ma.description->viewport(), QColor(162, 205, 90));
	}
      else if(str.startsWith("650"))
	{
	  /*
	  ** $a - Topical Term or Geographic Name as Entry Element
	  ** $b - Topical Term following Geographic Name as Entry
	  **      Element
	  ** $c - Location of Event
	  ** $d - Active Dates
	  ** $e - Relator Term
	  ** $v - Form Subdivision
	  ** $x - General Subdivision
	  ** $y - Chronological Subdivision
	  ** $z - Geographic Subdivision
	  ** $0 - Authority Record Control Number
	  ** $2 - Source of Heading or Term
	  ** $3 - Materials Specified
	  ** $4 - Relator Code
	  ** $6 - Linkage
	  ** $8 - Field Link and Sequence Number
	  */

	  str = str.mid(str.indexOf("$a") + 2).trimmed();
	  str = str.mid(0, str.indexOf("$v")).trimmed();

	  if(str.contains("$v"))
	    str = str.mid(0, str.indexOf("$v")).trimmed();

	  if(str.contains("$x"))
	    str = str.mid(0, str.indexOf("$x")).trimmed();

	  if(str.contains("$y"))
	    str = str.mid(0, str.indexOf("$y")).trimmed();

	  if(str.contains("$z"))
	    str = str.mid(0, str.indexOf("$z")).trimmed();

	  if(!str.isEmpty())
	    {
	      if(!str[str.length() - 1].isPunct())
		str += ".";

	      if(!ma.category->toPlainText().contains(str))
		{
		  if(!ma.category->toPlainText().isEmpty())
		    ma.category->setPlainText
		      (ma.category->toPlainText() + "\n" + str);
		  else
		    ma.category->setPlainText(str);

		  misc_functions::highlightWidget
		    (ma.category->viewport(), QColor(162, 205, 90));
		}
	    }
	}
    }

  foreach(QLineEdit *textfield, findChildren<QLineEdit *>())
    textfield->setCursorPosition(0);
}

/*
** -- slotSelectImage() --
*/

void qtbook_magazine::slotSelectImage(void)
{
  QFileDialog dialog(this);
  QPushButton *button = qobject_cast<QPushButton *> (sender());

  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setFilter("Image Files (*.bmp *.jpg *.jpeg *.png)");

  if(button == ma.frontButton)
    dialog.setWindowTitle(tr("BiblioteQ: Front Cover Image Selection"));
  else
    dialog.setWindowTitle(tr("BiblioteQ: Back Cover Image Selection"));

  dialog.exec();

  if(dialog.result() == QDialog::Accepted)
    {
      if(button == ma.frontButton)
	{
	  ma.front_image->clear();
	  ma.front_image->image = QImage(dialog.selectedFiles().at(0));
	  ma.front_image->imageFormat = dialog.selectedFiles().at(0).mid
	    (dialog.selectedFiles().at(0).lastIndexOf(".") + 1).toUpper();
	  ma.front_image->scene()->addPixmap
	    (QPixmap().fromImage(ma.front_image->image));
	  ma.front_image->scene()->items().at(0)->setFlags
	    (QGraphicsItem::ItemIsSelectable);
	}
      else
	{
	  ma.back_image->clear();
	  ma.back_image->image = QImage(dialog.selectedFiles().at(0));
	  ma.back_image->imageFormat = dialog.selectedFiles().at(0).mid
	    (dialog.selectedFiles().at(0).lastIndexOf(".") + 1).toUpper();
	  ma.back_image->scene()->addPixmap
	    (QPixmap().fromImage(ma.back_image->image));
	  ma.back_image->scene()->items().at(0)->setFlags
	    (QGraphicsItem::ItemIsSelectable);
	}
    }
}
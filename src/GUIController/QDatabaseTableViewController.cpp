/*
 * QDatabaseTableViewController.cpp
 *
 *  Created on: 14 juin 2016
 *      Author: echopin
 */

#include <GUIController/QDatabaseTableViewController.h>
#include <GUI/QDatabaseTableView.h>
#include "Database/DatabaseController.h"
#include <QDebug>
#include <QObject>
#include <QTextCursor>

QDatabaseTableViewController::QDatabaseTableViewController()
{
	m_pDatabaseTableView = NULL;
	m_szTableName = "";
	m_pDatabaseController = NULL;
}

QDatabaseTableViewController::~QDatabaseTableViewController()
{

}

void QDatabaseTableViewController::init(QDatabaseTableView* pDatabaseTableView, QString& szTableName, DatabaseController* pDatabaseController)
{
	m_pDatabaseTableView = pDatabaseTableView;
	m_szTableName = szTableName;
	m_pDatabaseController = pDatabaseController;

	connect(m_pDatabaseTableView->getRefreshButton(), SIGNAL(clicked()), this, SLOT(updateView()));
	connect(m_pDatabaseTableView->getClearButton(), SIGNAL(clicked()), this, SLOT(clearFilterField()));

	m_pDatabaseController->loadTableDescription(m_szTableName, onDbLoadTableDescription, this);
	showQueryInformation();
	updateView();
	showQueryInformation();
	m_pDatabaseController->loadTableCreationScript(m_szTableName, onDbLoadTableCreationScript, this);
}


void QDatabaseTableViewController::updateView()
{
	QString szFilter = m_pDatabaseTableView->getFilterLine()->text();
	m_pDatabaseTableView->getDataResultsModel()->clear();//Clear the table

	m_pDatabaseController->loadTableData(m_szTableName, szFilter, onDbLoadTableData, this);//Load the data again
	showQueryInformation();
}
void QDatabaseTableViewController::clearFilterField()
{
	m_pDatabaseTableView->getFilterLine()->clear();
	updateView();
}

void QDatabaseTableViewController::showQueryInformation()
{
	QString szQueryInformation = m_pDatabaseController->getQueryResultString();
	QTextCursor cursor(m_pDatabaseTableView->getConsoleTextEdit()->textCursor());//Creates a cursor
	cursor.movePosition(QTextCursor::Start);//Moves the cursor to the start
	m_pDatabaseTableView->getConsoleTextEdit()->setTextCursor(cursor);//Sets the cursor position
	m_pDatabaseTableView->getConsoleTextEdit()->insertPlainText(szQueryInformation);//insert text at the cursor position
}

void QDatabaseTableViewController::onDbLoadTableDescription(const QList<QString>& pColumnName, const QList<QString>& pRowData, void* user_data)
{
	QDatabaseTableViewController* pDatabaseTableViewController = (QDatabaseTableViewController*)(user_data);

	QList<QString> pHeader;
	pHeader << pColumnName;
	pDatabaseTableViewController->m_pDatabaseTableView->getStructureModel()->setHorizontalHeaderLabels(pHeader);
	QStandardItem* tableItemList;

	//Creating a QList<QStandardItem> in order to append a row to the model
	QList<QStandardItem*> pRowDataItemList;
	QList<QString>::const_iterator iter = pRowData.begin();
	while(iter != pRowData.end())
	{
		//Getting an item from QList<QString> to add it to a QList<QStandardItem>
		tableItemList = new QStandardItem(*iter);
		tableItemList->setEditable(false);
		pRowDataItemList.append(tableItemList);
		iter++;
	}

	//Adds a row to the table
	pDatabaseTableViewController->m_pDatabaseTableView->getStructureModel()->appendRow(pRowDataItemList);
}

void QDatabaseTableViewController::onDbLoadTableData(const QList<QString>& pColumnName, const QList<QString>& pRowData, void* user_data)
{
	QDatabaseTableViewController* pDatabaseTableViewController = (QDatabaseTableViewController*)(user_data);
	QList<QString> pHeader;

	//Adds rowid column to the header
	pHeader << pColumnName;
	pDatabaseTableViewController->m_pDatabaseTableView->getDataResultsModel()->setHorizontalHeaderLabels(pHeader);

	//Creating a QList<QStandardItem> in order to append a row to the model
	QList<QStandardItem*> pRowDataItemList;
	QList<QString>::const_iterator iter = pRowData.begin();
	while(iter != pRowData.end())
	{
		//Getting an item from QList<QString> to add it to a QList<QStandardItem>
		QStandardItem* pDataItem = new QStandardItem(*iter);
		pDataItem->setEditable(false);
		pRowDataItemList.append(pDataItem);
		iter++;
	}

	//Appending the row with the QList<QStandardItem>
	pDatabaseTableViewController->m_pDatabaseTableView->getDataResultsModel()->appendRow(pRowDataItemList);
}

void QDatabaseTableViewController::onDbLoadTableCreationScript(const QString& szCreationScriptString, void* user_data)
{
	QDatabaseTableViewController* pDatabaseTableViewController = (QDatabaseTableViewController*)(user_data);
	pDatabaseTableViewController->m_pDatabaseTableView->getCreationScriptTextEdit()->append(szCreationScriptString);
}

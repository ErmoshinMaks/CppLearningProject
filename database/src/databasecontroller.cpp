#include "databasecontroller.h"

DatabaseController::DatabaseController()
{
    this->connection();
    dbWindow = new DataWindow;

     connect(dbWindow, SIGNAL(sig_typeTable_clicked(Table)), this,SLOT(slot_typeTable_clicked(Table)));
     connect(this, SIGNAL(sig_table(QSqlTableModel *)), dbWindow,SLOT(slot_table(QSqlTableModel *)));
     connect(dbWindow, SIGNAL(sig_addButton_clicked()), this,SLOT(slot_addButton_clicked()));
     connect(dbWindow, SIGNAL(sig_deleteButton_clicked()), this,SLOT(slot_deleteButton_clicked()));
     connect(dbWindow, SIGNAL(sig_tableView_clicked(const QModelIndex &)), this,SLOT(slot_tableView_clicked(const QModelIndex &)));
     connect(this, SIGNAL(sig_block_db()), dbWindow,SLOT(slot_block_db()));
     connect(this, SIGNAL(sig_unblock_db()), dbWindow,SLOT(slot_unblock_db()));
}

int DatabaseController::connection()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("TECHNICALFEATURES.db");


    if (db.open())
    {
        qInfo() << "Database is open!";
        model = new QSqlTableModel(NULL,db);
        return 0;
    }
    else {
        qInfo() << "Can not open database! "<<db.lastError().databaseText();
        return -1;
    }

}

InfoAboutElement DatabaseController::select(Table table,int id)
{
    if(table == AIRPLANS)
    {
        return airplanTable->select(id);
    }
    else if(table == ZRK)
    {
        return zrkTable->select(id);
    }

}


QVector<InfoAboutElement> DatabaseController::select_all(Table table)
{
    QVector<InfoAboutElement> ans;
    if(table == AIRPLANS)
    {
        ans = airplanTable->select_all();
    }
    else if(table == ZRK)
    {
        ans = zrkTable->select_all();
    }
    return ans;
}

void DatabaseController::dataWindow_show()
{
    dbWindow->show();
}



void DatabaseController::slot_typeTable_clicked(Table table)
{
    switch (table)
    {
        case AIRPLANS:
            model->setTable("AIRPLANS");
            break;
        case ZRK:
            model->setTable("ZRK");
            break;

        default:
            break;
    }

    model->select();
    emit sig_table(model);
}

void DatabaseController::slot_addButton_clicked()
{
    model->insertRow(model->rowCount());
    emit sig_addedToDb();
}

void DatabaseController::slot_deleteButton_clicked()
{
        model->removeRow(currentRow);
        model->select();
}

void DatabaseController::slot_tableView_clicked(const QModelIndex &index)
{
    currentRow = index.row();
}

void DatabaseController::slot_block_db()
{
    emit sig_block_db();
}

void DatabaseController::slot_unblock_db()
{
    emit sig_unblock_db();
}

QSqlDatabase DatabaseController::return_db()
{
    return db;
}

int DatabaseController::closing()
{
     db.close();
     qInfo() << "Database is closed!";

     return 0;
}
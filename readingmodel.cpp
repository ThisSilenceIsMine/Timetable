#include "readingmodel.h"

ReadingModel::ReadingModel(QSqlDatabase* db)
    :m_db(db)
{
    QSqlQuery query(*m_db);
    ItemsList list;
    query.prepare("SELECT Teacher.PIB, Discipline.name FROM Reading "
                  "INNER JOIN Teacher ON Teacher.id = Reading.id_teacher "
                  "INNER JOIN Discipline ON Discipline.id = Reading.id_discipline "
                  );

    if(!query.exec())
    {
        qDebug() << query.lastQuery();
    }
    while (query.next()) {
        Item item;
        item.teacherName = query.value(0).toString();
        item.lessonName = query.value(1).toString();
        qDebug() << item.lessonName << item.teacherName;
        this->append(item);
    }
    query.finish();
}


void ReadingModel::append(const Item &item)
{
    if(insertRows(this->rowCount({}),1,{}))
        qDebug() << "Appending at " << this->rowCount({});

    if(this->rowCount({}) == 0)
    {
        qDebug() << "fuck! model is empty!";
    }
    bool ok = 0;

    ok |= setData(this->index(this->rowCount({}) - 1), QVariant(item.lessonName), RoleNames::LessonRole);
    ok |= setData(this->index(this->rowCount({}) - 1), QVariant(item.teacherName), RoleNames::TeacherRole);

    if(!ok)
        qDebug() << "Set data not working!";

}




int ReadingModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return m_data.count();
}

QVariant ReadingModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    Item item = m_data.at( index.row() );

    switch (role) {
    case LessonRole:
        qDebug() << Q_FUNC_INFO << "returning" << item.lessonName;
        return item.lessonName;
    case TeacherRole:
        qDebug() << Q_FUNC_INFO << "returning" << item.lessonName;
        return item.teacherName;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ReadingModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[LessonRole] = "lesson";
    roles[TeacherRole] = "teacher";
    return roles;
}


bool ReadingModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if(parent.isValid()) {
        return false;
    }
    beginInsertRows(QModelIndex(),row,row+count-1);

    for(int i = 0; i < count; ++i) {
        m_data.insert(row,{});
    }

    endInsertRows();
    return true;
}


bool ReadingModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid() || m_data.isEmpty()) {
        return false;
    }

    Item item = m_data.at(index.row());

    switch (role) {
        case RoleNames::LessonRole:
            item.lessonName = value.toString();
        break;
        case RoleNames::TeacherRole:
            item.teacherName = value.toString();
        break;
    }

    if(m_data.setDataAt(index.row(),item)) {
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }

    return false;
}

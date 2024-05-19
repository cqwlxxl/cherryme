#include "InterfacePublicData.h"

InterfacePublicData::InterfacePublicData(QObject *parent)
    : QObject{parent}
{
    //关联信号槽
    connect(this, &InterfacePublicData::SIGNALSendQuery, this, &InterfacePublicData::slotSendQuery, Qt::DirectConnection);
    connect(this, &InterfacePublicData::SIGNALReceiveQueryData, this, &InterfacePublicData::slotReceiveQueryData, Qt::UniqueConnection);
}

///发送到数据库前执行的操作
void InterfacePublicData::slotSendQuery(SqlOperateType operate, QVariant var)
{
    Q_UNUSED(var)
    switch(operate)
    {
    default:
        break;
    }
}

///接收sql结果槽函数
void InterfacePublicData::slotReceiveQueryData(SqlOperateType operate, QVariant var)
{
    Q_UNUSED(operate)
    Q_UNUSED(var)
}

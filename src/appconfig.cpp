#include "appconfig.h"
#include "global.h"
#include <QFile>
#include <QDebug>
#include <QColor>
#include <QDomDocument>
#include <QXmlStreamWriter>

AppConfig::AppConfig(QString fileName)
    : ch1IPAddress_("")
    , ch1PortNum_("")
    , ch1DataType_(DataType::Groundtruth)
    , ch1LoadFile_("")

    , ch2IPAddress_("")
    , ch2PortNum_("")
    , ch2DataType_(DataType::Groundtruth)
    , ch2LoadFile_("")

    , showGrid_(true)
    , showObjectDetails_(true)
    , lockCamera_(false)
    , laneType_(LaneType::BoundaryLanes)
    , typeColors_()

    , osiMsgSaveThreshold_(100)
    , srcPath_("./")

    , configFileName_(fileName)
{

}

bool
AppConfig::Load()
{
    QFile file(configFileName_);
    if (!file.open(QIODevice::ReadOnly))
    {
        // TODO: Don't use qDebug for printing
        qDebug() << "Error while loading config file: '" << configFileName_ << "'";
        return false;
    }

    QDomDocument dom;
    dom.setContent(&file);
    file.close();

    QDomElement root = dom.documentElement().toElement();

    ch1IPAddress_ = root.elementsByTagName("CH1IpAddress").at(0).toElement().text();
    ch1PortNum_   = root.elementsByTagName("CH1PortNumber").at(0).toElement().text();
    ch1DataType_  = (DataType)root.elementsByTagName("CH1DataType").at(0).toElement().text().toInt();
    ch1LoadFile_  = root.elementsByTagName("CH1LoadFile").at(0).toElement().text();

    ch2IPAddress_ = root.elementsByTagName("CH2IpAddress").at(0).toElement().text();
    ch2PortNum_   = root.elementsByTagName("CH2PortNumber").at(0).toElement().text();
    ch2DataType_  = (DataType)root.elementsByTagName("CH2DataType").at(0).toElement().text().toInt();
    ch2LoadFile_  = root.elementsByTagName("CH2LoadFile").at(0).toElement().text();

    showGrid_     = root.elementsByTagName("ShowGrid").at(0).toElement().text() == "1" ? true : false;
    showObjectDetails_ = root.elementsByTagName("ShowObjectDetails").at(0).toElement().text() == "1" ? true : false;
    lockCamera_   = root.elementsByTagName("LockCamera").at(0).toElement().text() == "1" ? true : false;
    laneType_     = root.elementsByTagName("LaneType").at(0).toElement().text() == "0" ? LaneType::BoundaryLanes : LaneType::CenterLanes;

    QList<ObjectType> types = Global::GetAllObjectTypes();
    foreach (ObjectType type, types)
    {
        QString typeName = Global::GetObjectTypeName(type);
        QString color = root.elementsByTagName(typeName).at(0).toElement().text();
        typeColors_.insert(type, color);
    }

    return true;
}

bool
AppConfig::Save()
{
    QFile file(configFileName_);
    if (!file.open(QIODevice::WriteOnly))
    {
        // TODO: Don't use qDebug for printing
        qDebug() << "Error while saving config file: '" << configFileName_ << "'";
        return false;
    }

    QString output;
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);

    writer.writeStartDocument();
    writer.writeStartElement("AppConfig");

    writer.writeTextElement("CH1IpAddress", ch1IPAddress_);
    writer.writeTextElement("CH1PortNumber", ch1PortNum_);
    writer.writeTextElement("CH1DataType", QString::number(static_cast<int>(ch1DataType_)));
    writer.writeTextElement("CH1LoadFile", ch1LoadFile_);

    writer.writeTextElement("CH2IpAddress", ch2IPAddress_);
    writer.writeTextElement("CH2PortNumber", ch2PortNum_);
    writer.writeTextElement("CH2DataType", QString::number(static_cast<int>(ch2DataType_)));
    writer.writeTextElement("CH2LoadFile", ch2LoadFile_);

    writer.writeTextElement("ShowGrid", QString::number(showGrid_));
    writer.writeTextElement("ShowObjectDetails", QString::number(showObjectDetails_));
    writer.writeTextElement("LockCamera", QString::number(lockCamera_));
    writer.writeTextElement("LaneType", QString::number(static_cast<int>(laneType_)));

    foreach (ObjectType key, typeColors_.keys())
    {
        writer.writeTextElement(Global::GetObjectTypeName(key), typeColors_.value(key).name());
    }

    writer.writeEndElement();
    writer.writeEndDocument();

    QTextStream stream(&file);
    stream << output;
    file.close();

    return true;
}


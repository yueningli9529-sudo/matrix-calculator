#include "matrixitemdelegate.h"
#include "matrixtablemodel.h"
#include <QApplication>
#include <QPainter>
#include <QStyle>
#include <QWidget>
#include <QFontMetrics>

MatrixItemDelegate::MatrixItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void MatrixItemDelegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    if(index.data(Qt::DisplayRole).isValid())
    {
        QStyledItemDelegate::paint(painter,option,index);
    }
    else
    {
        QVariant Qnumer = index.data(MatrixTableModel::NumeratorRole);
        QVariant Qdenom = index.data(MatrixTableModel::DenominatorRole);
        if(!Qnumer.isValid() || !Qdenom.isValid())
        {
            return;
        }
        QString Snumer = Qnumer.toString();
        QString Sdenom = Qdenom.toString();

        QStyleOptionViewItem opt = option;
        initStyleOption(&opt,index);
        opt.text.clear();

        QStyle* style = opt.widget? opt.widget->style(): QApplication::style();

        painter->save();

        //draw background
        style->drawControl(
            QStyle::CE_ItemViewItem,
            &opt,
            painter,
            opt.widget);

        // size calculation
        const QFontMetrics metrics(opt.font);
        const int numerWidth = metrics.horizontalAdvance(Snumer);
        const int denomWidth = metrics.horizontalAdvance(Sdenom);
        const int fractionWidth = qMax(numerWidth,denomWidth) + 8;
        const int textHeight = metrics.height();
        const int centerX = opt.rect.center().x();
        const int centerY = opt.rect.center().y();
        //inside rect
        QRect numerRect(
            centerX-fractionWidth/2,
            centerY - textHeight - 2,
            fractionWidth,
            textHeight);
        QRect denomRect(
            centerX - fractionWidth/2,
            centerY + 2,
            fractionWidth,
            textHeight);
        //set painter
        painter->setFont(opt.font);
        if(opt.state & QStyle::State_Selected)
        {
            painter->setPen(opt.palette.highlightedText().color());
        }
        else
        {
            painter->setPen(opt.palette.text().color());
        }
        //paint
        painter->drawText(
            numerRect,
            Qt::AlignCenter,
            Snumer);
        painter->drawLine(
            centerX - fractionWidth/2,
            centerY,
            centerX + fractionWidth/2,
            centerY);
        painter->drawText(
            denomRect,
            Qt::AlignCenter,
            Sdenom);
        painter->restore();
    }
}

QSize MatrixItemDelegate::sizeHint(
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    if(index.data(Qt::DisplayRole).isValid())
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }
    QVariant numerator = index.data(MatrixTableModel::NumeratorRole);
    QVariant denominator = index.data(MatrixTableModel::DenominatorRole);
    if(!numerator.isValid() || !denominator.isValid())
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }
    const QFontMetrics metrics(option.font);
    const int width = qMax(metrics.horizontalAdvance(numerator.toString()),
                           metrics.horizontalAdvance(denominator.toString())) +16;
    const int height = metrics.height() * 2 + 10;
    return QSize(width,height);
}

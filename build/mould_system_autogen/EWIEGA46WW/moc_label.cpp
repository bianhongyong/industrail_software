/****************************************************************************
** Meta object code from reading C++ file 'label.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../label.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'label.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_label_t {
    QByteArrayData data[24];
    char stringdata0[355];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_label_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_label_t qt_meta_stringdata_label = {
    {
QT_MOC_LITERAL(0, 0, 5), // "label"
QT_MOC_LITERAL(1, 6, 23), // "on_pushButton_3_clicked"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 21), // "on_not_labeld_clicked"
QT_MOC_LITERAL(4, 53, 17), // "on_labeld_clicked"
QT_MOC_LITERAL(5, 71, 31), // "on_file_list_currentItemChanged"
QT_MOC_LITERAL(6, 103, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(7, 120, 7), // "current"
QT_MOC_LITERAL(8, 128, 8), // "previous"
QT_MOC_LITERAL(9, 137, 10), // "paint_rect"
QT_MOC_LITERAL(10, 148, 5), // "start"
QT_MOC_LITERAL(11, 154, 3), // "end"
QT_MOC_LITERAL(12, 158, 26), // "update_annotationlist_slot"
QT_MOC_LITERAL(13, 185, 12), // "category_idx"
QT_MOC_LITERAL(14, 198, 4), // "text"
QT_MOC_LITERAL(15, 203, 21), // "not_delete_label_slot"
QT_MOC_LITERAL(16, 225, 10), // "label_name"
QT_MOC_LITERAL(17, 236, 17), // "paint_update_slot"
QT_MOC_LITERAL(18, 254, 14), // "paint_original"
QT_MOC_LITERAL(19, 269, 22), // "delete_annotation_slot"
QT_MOC_LITERAL(20, 292, 4), // "item"
QT_MOC_LITERAL(21, 297, 27), // "delete_all_annotations_slot"
QT_MOC_LITERAL(22, 325, 21), // "on_not_labeld_toggled"
QT_MOC_LITERAL(23, 347, 7) // "checked"

    },
    "label\0on_pushButton_3_clicked\0\0"
    "on_not_labeld_clicked\0on_labeld_clicked\0"
    "on_file_list_currentItemChanged\0"
    "QListWidgetItem*\0current\0previous\0"
    "paint_rect\0start\0end\0update_annotationlist_slot\0"
    "category_idx\0text\0not_delete_label_slot\0"
    "label_name\0paint_update_slot\0"
    "paint_original\0delete_annotation_slot\0"
    "item\0delete_all_annotations_slot\0"
    "on_not_labeld_toggled\0checked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_label[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x08 /* Private */,
       3,    0,   75,    2, 0x08 /* Private */,
       4,    0,   76,    2, 0x08 /* Private */,
       5,    2,   77,    2, 0x08 /* Private */,
       9,    2,   82,    2, 0x08 /* Private */,
      12,    4,   87,    2, 0x08 /* Private */,
      15,    1,   96,    2, 0x08 /* Private */,
      17,    0,   99,    2, 0x08 /* Private */,
      18,    0,  100,    2, 0x08 /* Private */,
      19,    1,  101,    2, 0x08 /* Private */,
      21,    0,  104,    2, 0x08 /* Private */,
      22,    1,  105,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 6,    7,    8,
    QMetaType::Void, QMetaType::QPoint, QMetaType::QPoint,   10,   11,
    QMetaType::Void, QMetaType::QPoint, QMetaType::QPoint, QMetaType::QString, QMetaType::QString,   10,   11,   13,   14,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,   20,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   23,

       0        // eod
};

void label::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<label *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_pushButton_3_clicked(); break;
        case 1: _t->on_not_labeld_clicked(); break;
        case 2: _t->on_labeld_clicked(); break;
        case 3: _t->on_file_list_currentItemChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 4: _t->paint_rect((*reinterpret_cast< const QPoint(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2]))); break;
        case 5: _t->update_annotationlist_slot((*reinterpret_cast< const QPoint(*)>(_a[1])),(*reinterpret_cast< const QPoint(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 6: _t->not_delete_label_slot((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->paint_update_slot(); break;
        case 8: _t->paint_original(); break;
        case 9: _t->delete_annotation_slot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 10: _t->delete_all_annotations_slot(); break;
        case 11: _t->on_not_labeld_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject label::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_label.data,
    qt_meta_data_label,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *label::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *label::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_label.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int label::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

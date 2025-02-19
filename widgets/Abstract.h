#ifndef ABSTRACT_H
#define ABSTRACT_H


/**该文件包含所有抽象接口**/
#include <QMenu>
#include <QListWidget>
#include <global.h>

class CustomListWidget;  // 前向声明，解决循环依赖

/**菜单抽象**/
class IContextMenuProvider {
public:
    virtual ~IContextMenuProvider() {}
    virtual void populateMenu(QMenu *menu, QListWidgetItem *item, CustomListWidget *one_list) = 0;
};



/*具体菜单类*/
class Annoation_ContextMenuProvider : public IContextMenuProvider {
public:
    void populateMenu(QMenu *menu, QListWidgetItem *item,CustomListWidget *one_list) override;
};

class Label_ContextMenuProvider : public IContextMenuProvider {
public:
    void populateMenu(QMenu *menu, QListWidgetItem *item,CustomListWidget *one_list) override;
};

class file_ContextMenuProvider : public IContextMenuProvider {
public:
    void populateMenu(QMenu *menu, QListWidgetItem *item,CustomListWidget *one_list) override;
};

#endif // ABSTRACT_H

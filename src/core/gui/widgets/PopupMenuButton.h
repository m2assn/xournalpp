/*
 * Xournal++
 *
 * Button with Popup Menu
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <string>
#include <vector>

#include <gtk/gtk.h>


class PopupMenuButton {
public:
    PopupMenuButton(GtkWidget* button, GMenu* menu);
    virtual ~PopupMenuButton();

public:
    void setMenu(GtkWidget* menu);

private:
    GtkWidget* button;
    GMenu* menu;
};

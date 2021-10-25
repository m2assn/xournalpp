/*
 * Xournal++
 *
 * The main Control
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

#include "control/tools/EditSelection.h"


class ObjectInputStream;

struct ClipboardListener {
    virtual void clipboardCutCopyEnabled(bool enabled) = 0;
    virtual void clipboardPasteEnabled(bool enabled) = 0;
    virtual void clipboardPasteText(std::string text) = 0;
    virtual void clipboardPasteImage(GdkPaintable* img) = 0;
    virtual void clipboardPasteXournal(ObjectInputStream& in) = 0;
    virtual void deleteSelection() = 0;

    virtual ~ClipboardListener() = default;

protected:
    ClipboardListener() = default;
};

class ClipboardHandler {
public:
    ClipboardHandler(ClipboardListener* listener, GtkWidget* widget);
    virtual ~ClipboardHandler();

public:
    bool paste();
    bool cut();
    bool copy();

    void setSelection(EditSelection* selection);

    void setCopyPasteEnabled(bool enabled);

private:
    static void ownerChangedCallback(GdkClipboard* clip, ClipboardHandler* handler);
    void clipboardUpdated();

private:
    ClipboardListener* listener = nullptr;
    GdkClipboard* clipboard = nullptr;
    gulong hanlderId = -1;

    EditSelection* selection = nullptr;

    bool containsText = false;
    bool containsXournal = false;
    bool containsImage = false;
};

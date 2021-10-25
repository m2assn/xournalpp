/*
 * Xournal++
 *
 * Part of the customizable toolbars
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <string>
#include <vector>

#include "gui/IconNameHelper.h"

#include "AbstractToolItem.h"


class GladeGui;
class SpinPageAdapter;

class ToolPageSpinner: public AbstractToolItem {
public:
    ToolPageSpinner(GladeGui* gui, ActionHandler* handler, std::string id, ActionType type,
                    IconNameHelper iconNameHelper);
    ~ToolPageSpinner() override;

public:
    SpinPageAdapter* getPageSpinner() const;
    void setPageInfo(size_t pagecount, size_t pdfpage);
    std::string getToolDisplayName() const override;
    GtkButton* createItem(bool horizontal) override;
    GtkButton* createTmpItem(bool horizontal) override;

protected:
    GtkButton* newItem() override;
    GtkWidget* getNewToolIcon() const override;
    GdkPixbuf* getNewToolPixbuf() const override;

private:
    void updateLabels();

private:
    GladeGui* gui = nullptr;

    SpinPageAdapter* pageSpinner = nullptr;
    GtkOrientation orientation = GTK_ORIENTATION_HORIZONTAL;

    GtkWidget* box = nullptr;
    GtkWidget* lbPageNo = nullptr;
    GtkWidget* lbVerticalPdfPage = nullptr;

    /** The current page of the document. */
    size_t pageCount = 0;

    /** The current page in the background PDF, or 0 if there is no PDF. */
    size_t pdfPage = 0;

    IconNameHelper iconNameHelper;
};

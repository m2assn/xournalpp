#include "ColorToolItem.h"

#include <cinttypes>

#include <config.h>

#include "control/ToolEnums.h"
#include "gui/toolbarMenubar/icon/ColorSelectImage.h"
#include "util/GtkDialogUtil.h"
#include "util/StringUtils.h"
#include "util/Util.h"
#include "util/i18n.h"

bool ColorToolItem::inUpdate = false;

ColorToolItem::ColorToolItem(ActionHandler* handler, ToolHandler* toolHandler, GtkWindow* parent, NamedColor namedColor,
                             bool selektor):
        AbstractToolItem("", handler, selektor ? ACTION_SELECT_COLOR_CUSTOM : ACTION_SELECT_COLOR),
        toolHandler(toolHandler),
        namedColor{std::move(namedColor)} {
    this->group = GROUP_COLOR;
}

ColorToolItem::~ColorToolItem() { freeIcons(); }

/**
 * Free the allocated icons
 */
void ColorToolItem::freeIcons() {
    delete this->icon;
    this->icon = nullptr;
}

auto ColorToolItem::isSelector() const -> bool { return this->action == ACTION_SELECT_COLOR_CUSTOM; }


void ColorToolItem::actionSelected(ActionGroup group, ActionType action) {
    inUpdate = true;
    if (this->group == group && this->item) {
        if (isSelector()) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(this->item), isSelector());
        }
        enableColor(toolHandler->getColor());
    }
    inUpdate = false;
}

void ColorToolItem::enableColor(Color color) {
    if (isSelector()) {
        if (this->icon) {
            this->icon->setColor(color);
        }

        this->namedColor = NamedColor{color};
        if (GTK_IS_TOGGLE_BUTTON(this->item)) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(this->item), false);
        }
    } else {
        if (this->item) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(this->item), this->namedColor.getColor() == color);
        }
    }
}

auto ColorToolItem::getColor() const -> Color { return this->namedColor.getColor(); }

auto ColorToolItem::getId() const -> std::string {
    if (isSelector()) {
        return "COLOR_SELECT";
    }

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "COLOR(%zu)", this->namedColor.getIndex());
    std::string id = buffer;

    return id;
}

/**
 * Show colochooser to select a custom color
 */
void ColorToolItem::showColorchooser() {
    GtkWidget* dialog = gtk_color_chooser_dialog_new(_("Select color"), parent);
    gtk_color_chooser_set_use_alpha(GTK_COLOR_CHOOSER(dialog), false);

    int response = wait_for_gtk_dialog_result(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        GdkRGBA color;
        gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(dialog), &color);
        this->namedColor = NamedColor{Util::GdkRGBA_to_argb(color)};
    }

    gtk_window_destroy(GTK_WINDOW(dialog));
}

/**
 * Enable / Disable the tool item
 */
void ColorToolItem::enable(bool enabled) {
    if (!enabled && !toolHandler->hasCapability(TOOL_CAP_COLOR, SelectedTool::active) &&
        toolHandler->hasCapability(TOOL_CAP_COLOR, SelectedTool::toolbar)) {
        if (this->icon) {
            /*
             * allow changes if currentTool has no colour capability
             * and mainTool has Colour capability
             */
            icon->setState(COLOR_ICON_STATE_PEN);
            AbstractToolItem::enable(true);
        }
        return;
    }

    AbstractToolItem::enable(enabled);
    if (this->icon) {
        if (enabled) {
            icon->setState(COLOR_ICON_STATE_ENABLED);
        } else {
            icon->setState(COLOR_ICON_STATE_DISABLED);
        }
    }
}

void ColorToolItem::activated(GdkEvent* event, GtkButton* menuitem, GtkButton* toolbutton) {
    if (inUpdate) {
        return;
    }
    inUpdate = true;

    if (isSelector()) {
        showColorchooser();
    }

    toolHandler->setColor(this->namedColor.getColor(), true);

    inUpdate = false;
}

auto ColorToolItem::newItem() -> GtkWidget* {
    this->icon = new ColorSelectImage(this->namedColor.getColor(), !isSelector());

    GtkWidget* it = gtk_toggle_button_new();

    const gchar* name = this->namedColor.getName().c_str();
    gtk_widget_set_tooltip_text(it, name);
    gtk_button_set_label(GTK_BUTTON(it), name);
    gtk_button_set_child(GTK_BUTTON(it), this->icon->getWidget());

    return it;
}

auto ColorToolItem::getToolDisplayName() const -> std::string { return this->namedColor.getName(); }

auto ColorToolItem::getNewToolIcon() const -> GtkWidget* {
    return ColorSelectImage::newColorIcon(this->namedColor.getColor(), 16, !isSelector());
}

auto ColorToolItem::getNewToolPixbuf() const -> GdkPixbuf* {
    return ColorSelectImage::newColorIconPixbuf(this->namedColor.getColor(), 16, !isSelector());
}

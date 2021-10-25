#include "SidebarPreviewLayerEntry.h"

#include "gui/Shadow.h"
#include "util/i18n.h"

#include "SidebarPreviewLayers.h"


SidebarPreviewLayerEntry::SidebarPreviewLayerEntry(SidebarPreviewBase* sidebar, const PageRef& page, int layer,
                                                   const std::string& layerName, size_t index, bool stacked):
        SidebarPreviewBaseEntry(sidebar, page),
        index(index),
        layer(layer),
        stacked(stacked),
        box(gtk_box_new(GTK_ORIENTATION_VERTICAL, 2)) {
    GtkWidget* toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    cbVisible = gtk_check_button_new_with_label(layerName.c_str());

    g_signal_connect(cbVisible, "toggled", G_CALLBACK(+[](GtkToggleButton* source, SidebarPreviewLayerEntry* self) {
                         self->checkboxToggled();
                     }),
                     this);


    // Left padding
    gtk_widget_set_margin_start(cbVisible, Shadow::getShadowTopLeftSize());

    gtk_box_append(GTK_BOX(toolbar), cbVisible);

    gtk_widget_set_vexpand(widget, false);
    gtk_box_append(GTK_BOX(box), widget);

    gtk_widget_set_vexpand(toolbar, false);
    gtk_box_append(GTK_BOX(box), toolbar);

    toolbarHeight = gtk_widget_get_allocated_height(cbVisible) + Shadow::getShadowTopLeftSize() + 20;
}

SidebarPreviewLayerEntry::~SidebarPreviewLayerEntry() { g_object_unref(this->box); }

void SidebarPreviewLayerEntry::checkboxToggled() {
    if (inUpdate) {
        return;
    }

    bool check = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cbVisible));
    (dynamic_cast<SidebarPreviewLayers*>(sidebar))->layerVisibilityChanged(layer + 1, check);
}

void SidebarPreviewLayerEntry::mouseButtonPressCallback() {
    (dynamic_cast<SidebarPreviewLayers*>(sidebar))->layerSelected(index);
}

auto SidebarPreviewLayerEntry::getRenderType() -> PreviewRenderType {
    return stacked ? RENDER_TYPE_PAGE_LAYERSTACK : RENDER_TYPE_PAGE_LAYER;
}

auto SidebarPreviewLayerEntry::getHeight() -> int { return getWidgetHeight() + toolbarHeight; }

auto SidebarPreviewLayerEntry::getLayer() const -> int { return layer; }

auto SidebarPreviewLayerEntry::getWidget() -> GtkWidget* { return this->box; }

/**
 * Set the value of the visible checkbox
 */
void SidebarPreviewLayerEntry::setVisibleCheckbox(bool enabled) {
    inUpdate = true;

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cbVisible), enabled);

    inUpdate = false;
}

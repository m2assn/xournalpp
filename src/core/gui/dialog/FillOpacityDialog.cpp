#include "FillOpacityDialog.h"

#include "GtkDialogUtil.h"
#include "pixbuf-utils.h"

FillOpacityDialog::FillOpacityDialog(GladeSearchpath* gladeSearchPath, int alpha, bool pen):
        GladeGui(gladeSearchPath, "fillOpacity.glade", "fillOpacityDialog"), pen(pen) {
    GtkWidget* scaleAlpha = get("scaleAlpha");

    gtk_range_set_value(GTK_RANGE(scaleAlpha), static_cast<int>(alpha / 255.0 * 100));

    setPreviewImage(alpha);

    g_signal_connect(scaleAlpha, "change-value",
                     G_CALLBACK(+[](GtkRange* range, GtkScrollType scroll, gdouble value, FillOpacityDialog* self) {
                         self->setPreviewImage((int)(value / 100 * 255));
                         gtk_range_set_value(range, value);
                     }),
                     this);
}

FillOpacityDialog::~FillOpacityDialog() = default;

const int PREVIEW_WIDTH = 70;
const int PREVIEW_HEIGTH = 50;
const int PREVIEW_BORDER = 10;

void FillOpacityDialog::setPreviewImage(int alpha) {
    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, PREVIEW_WIDTH, PREVIEW_HEIGTH);
    cairo_t* cr = cairo_create(surface);

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgb(cr, 255, 255, 255);
    cairo_paint(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_source_rgba(cr, 0, 0x80 / 255.0, 0, alpha / 255.0);
    cairo_rectangle(cr, PREVIEW_BORDER, PREVIEW_BORDER, PREVIEW_WIDTH - PREVIEW_BORDER * 2,
                    PREVIEW_HEIGTH - PREVIEW_BORDER * 2);
    cairo_fill(cr);

    if (pen) {
        cairo_set_line_width(cr, 5);
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
        cairo_set_source_rgb(cr, 0, 0x80 / 255.0, 0);
        cairo_rectangle(cr, PREVIEW_BORDER, PREVIEW_BORDER, PREVIEW_WIDTH - PREVIEW_BORDER * 2,
                        PREVIEW_HEIGTH - PREVIEW_BORDER * 2);
        cairo_stroke(cr);
    }

    cairo_destroy(cr);

    GtkWidget* preview = get("imgPreview");

    auto* pixbuf = xoj_pixbuf_get_from_surface(surface, 0, 0, PREVIEW_WIDTH, PREVIEW_WIDTH);
    gtk_image_set_from_pixbuf(GTK_IMAGE(preview), pixbuf);
    g_object_unref(pixbuf);
    g_object_unref(surface);

    cairo_surface_destroy(surface);
}

auto FillOpacityDialog::getResultAlpha() const -> int { return resultAlpha; }

void FillOpacityDialog::show(GtkWindow* parent) {
    gtk_window_set_transient_for(GTK_WINDOW(this->window), parent);
    int result = wait_for_gtk_dialog_result(GTK_DIALOG(this->window));
    gtk_widget_hide(this->window);

    // OK Button
    if (result == 1) {

        GtkWidget* scaleAlpha = get("scaleAlpha");
        resultAlpha = static_cast<int>(gtk_range_get_value(GTK_RANGE(scaleAlpha)) * 255.0 / 100.0);
    } else {
        resultAlpha = -1;
    }
}

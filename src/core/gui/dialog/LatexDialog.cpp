/*
 * Xournal++
 *
 * Latex implementation
 *
 * @author W Brenna
 * http://wbrenna.ca
 *
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#include "LatexDialog.h"

#include <utility>

#include "GtkDialogUtil.h"
#include "pixbuf-utils.h"

LatexDialog::LatexDialog(GladeSearchpath* gladeSearchPath): GladeGui(gladeSearchPath, "texdialog.glade", "texDialog") {
    this->texBox = get("texView");
    this->textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(this->texBox));
    this->texTempRender = get("texImage");

    // increase the maximum length to something reasonable.
    // gtk_entry_set_max_length(GTK_TEXT_BUFFER(this->texBox), 500);

    // Background color for the temporary render, default is white because
    // on dark themed DE the LaTex is hard to read
    this->cssProvider = gtk_css_provider_new();
    constexpr std::string_view css{"*{background-color:white;padding:10px;}"};
    gtk_css_provider_load_from_data(this->cssProvider, css.data(), css.size());
    gtk_style_context_add_provider(gtk_widget_get_style_context(this->texTempRender),
                                   GTK_STYLE_PROVIDER(this->cssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

LatexDialog::~LatexDialog() = default;

void LatexDialog::setFinalTex(std::string texString) { this->finalLatex = std::move(texString); }

auto LatexDialog::getFinalTex() -> std::string { return this->finalLatex; }

void LatexDialog::setTempRender(PopplerDocument* pdf) {
    if (poppler_document_get_n_pages(pdf) < 1) {
        return;
    }

    // If a previous render exists, destroy it
    if (this->scaledRender != nullptr) {
        cairo_surface_destroy(this->scaledRender);
        this->scaledRender = nullptr;
    }

    PopplerPage* page = poppler_document_get_page(pdf, 0);

    double zoom = 5;
    double pageWidth = 0;
    double pageHeight = 0;
    poppler_page_get_size(page, &pageWidth, &pageHeight);

    if ((pageWidth * zoom) > 1200) {
        zoom = 1200 / pageWidth;
    }

    auto const width = static_cast<int>(pageWidth * zoom);
    auto const height = static_cast<int>(pageHeight * zoom);


    this->scaledRender = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t* cr = cairo_create(this->scaledRender);

    cairo_scale(cr, zoom, zoom);

    poppler_page_render(page, cr);

    // Free resources.
    cairo_destroy(cr);
    g_clear_object(&page);

    // Update GTK widget

    auto* pixbuf = xoj_pixbuf_get_from_surface(this->scaledRender, 0, 0, width, height);
    gtk_image_set_from_pixbuf(GTK_IMAGE(this->texTempRender), pixbuf);
    g_object_unref(pixbuf);
}

auto LatexDialog::getTextBuffer() -> GtkTextBuffer* { return this->textBuffer; }

auto LatexDialog::getBufferContents() -> std::string {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(this->textBuffer, &start, &end);
    gchar* chars = gtk_text_buffer_get_text(this->textBuffer, &start, &end, false);
    std::string s = chars;
    g_free(chars);
    return s;
}

void LatexDialog::show(GtkWindow* parent) { this->show(parent, false); }

void LatexDialog::show(GtkWindow* parent, bool selectText) {
    gtk_text_buffer_set_text(this->textBuffer, this->finalLatex.c_str(), -1);
    if (selectText) {
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(this->textBuffer, &start, &end);
        gtk_text_buffer_select_range(this->textBuffer, &start, &end);
    }

    gtk_window_set_transient_for(GTK_WINDOW(this->window), parent);
    int res = wait_for_gtk_dialog_result(GTK_DIALOG(this->window));
    this->finalLatex = res == GTK_RESPONSE_OK ? this->getBufferContents() : "";

    gtk_widget_hide(this->window);
}

/*
 * Xournal++
 *
 * A geometry tool controller
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <gtk/gtk.h>

#include "gui/PageView.h"
#include "util/Point.h"

class GeometryTool;
class Stroke;

/**
 * @brief A class that controls a geometry tool

 * The geometry tool can be moved, rotated and scaled
 * There are methods for translating coordinates
 * and methods to deal with the temporary stroke
 */

class GeometryToolController {
public:
    GeometryToolController(XojPageView* view, GeometryTool* geometryTool);
    ~GeometryToolController();

public:
    /**
     * @brief moves the geometry tool in x- and y-direction
     * @param x the translation in x-direction (in document coordinates)
     * @param y the translation in y-direction (in document coordinates)
     */
    void move(double x, double y);

    /**
     * @brief rotates the geometry tool around its rotation center
     * @param da the rotation angle
     * @param cx the x-coordinate of the rotation center (in document coordinates)
     * @param cy the y-coordinate of the rotation center (in document coordinates)
     */
    void rotate(double da, double cx, double cy);

    /**
     * @brief resizes the geometry tool by the factor f with respect to a given scaling center
     * @param f the scaling factor
     * @param cx the x-coordinate of the scaling center (in document coordinates)
     * @param cy the y-coordinate of the scaling center (in document coordiantes)
     */
    void scale(double f, double cx, double cy);

    /**
     * @brief adds the stroke to the layer and rerenders the stroke area
     */
    void addStrokeToLayer();

    /**
     * @brief initializes the stroke by using the properties from the tool handler
     */
    void initializeStroke();

    /**
     * @brief the page view of the page with respect to which the geometry tool is initialized
     */
    XojPageView* getView() const;

    /**
     * @brief the page with respect to which the setsquare is initialized
     */
    PageRef getPage() const;

    virtual bool isInsideGeometryTool(double x, double y, double border = 0.0) const = 0;

    virtual std::string getType() const = 0;

public:
    XojPageView* view;

    /**
     * @brief the underlying geometry tool
     */
    GeometryTool* geometryTool;

    /**
     * @brief The stroke drawn
     */
    Stroke* stroke = nullptr;
};
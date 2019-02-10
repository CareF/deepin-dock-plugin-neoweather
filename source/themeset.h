#ifndef THEMESET_H
#define THEMESET_H
#include <QStringList>

/** \brief themeSet is a list describing the possible choices of icon themes.
 * The first String is the default choice.
 * The theme names should be consistent with the corresponding definition of
 * the prefix in `res.qrc` file.
 * In the `res.qrc` file it's expected to set alias of the icon codes:
 * `01d`, `01n`, ..., `04n`, `09d`, ..., `13d`, `13n`, `50d`, `50n`, `na`
 */
const QStringList themeSet({"neoGrayS", "neoGray", "neoColor", "hty"});

#endif // THEMESET_H

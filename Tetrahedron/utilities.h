#pragma once
/*!
          \brief Возвращает случайный цвет типа short в диапозоне от 1 до 6
          \details  ObjectARX Introductory Concepts -> Entities -> Common Entity
   Properties -> Entity Color
                                https://help.autodesk.com/view/OARX/2022/ENU/?guid=GUID-4A760CC5-F3CB-4EDD-B049-F62EB588A471
                                1 Red, 2 Yellow, 3 Green, 4 Cyan, 5 Blue, 6
   Magenta \return Цвет
*/
short getRandomColor();

/*!
        \brief Класс для идентификации grip точек, по переданному в конструкторе
   id. Используется в методе subGetGripPoints чтобы установить поле m_pAppData
   для класса AcDbGripData, чтобы в дальнейшем, в методе subMoveGripPointsAt
   получить id grip точки.
*/
class GripAppData final {
private:
  int m_Id; ///< идентификатор для AcDbGripData
public:
  GripAppData(int aId) : m_Id(aId) {}
  [[nodiscard]] int index() const noexcept { return m_Id; }
};
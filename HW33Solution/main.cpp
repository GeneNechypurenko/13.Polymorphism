#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

/*
==========================================================================================================================================
                         ДОМАШНЄ ЗАВДАННЯ 33 тема: поліморфізм — віртуальні методи, override і final, віртуальний деструктор.
==========================================================================================================================================
*/

// ==================== РІВЕНЬ 1. Зілля та інгредієнти ====================

class Potion
{
private:
    int healing;
    int toxicity;

public:
    Potion() : healing(0), toxicity(0)
    {}

    void addHealing(int amount)
    {
        healing += amount;
    }

    void addToxicity(int amount)
    {
        toxicity += amount;
    }

    void dilute()
    {
        toxicity /= 2;
    }

    void amplify()
    {
        healing *= 2;
        toxicity *= 2;
    }

    // Рівень 3
    void transmute()
    {
        healing += toxicity;
        toxicity = 0;
    }

    string getResult() const
    {
        if (healing + toxicity > 200)
        {
            return "Котел вибухнув!";
        }
        if (healing == 0 && toxicity == 0)
        {
            return "Брудна вода";
        }
        if (toxicity > healing)
        {
            return "Отрута";
        }
        return "Зілля лікування";
    }

    void print() const
    {
        cout << "Лікування: " << healing << ", токсичність: " << toxicity << " → " << getResult() << endl;
    }
};

class Ingredient
{
protected:
    string name;
    int power;

public:
    Ingredient(const string& name, int power) : name(name), power(power)
    {}

    /*
         5. (Рівень 2) Чому деструктор Ingredient віртуальний, хоча він нічого не робить? Де саме в програмі інгредієнт
            видаляється через покажчик на базовий клас?
    */
    virtual ~Ingredient() = default;

    /*
      2. getName() не віртуальний, а addTo() — віртуальний. Чому getName() не потрібно робити віртуальним?
         Що станеться з зіллям, якщо прибрати virtual в addTo() (і override у нащадках)?
    */
    string getName() const
    {
        return name;
    }

    // Інертна домішка нічого не змінює; справжні інгредієнти перевизначають цей метод
    virtual void addTo(Potion& potion) const
    {
        cout << "  " << name << " розчиняється без сліду" << endl;
    }
};

class Herb : public Ingredient
{
public:
    Herb(const string& name, int power) : Ingredient(name, power)
    {}

    void addTo(Potion& potion) const override
    {
        potion.addHealing(power);
        cout << "  " << name << ": +" << power << " до лікування" << endl;
    }
};

class Mushroom : public Ingredient
{
public:
    Mushroom(const string& name, int power) : Ingredient(name, power)
    {}

    //  3. Що буде, якщо в Mushroom написати void addTo(Potion& potion) без const: а) з override; б) без override?
    void addTo(Potion& potion) const override
    {
        potion.addToxicity(power);
        cout << "  " << name << ": +" << power << " до токсичності" << endl;
    }
};

class Water : public Ingredient
{
public:
    Water() : Ingredient("Чиста вода", 0)
    {}

    void addTo(Potion& potion) const override
    {
        potion.dilute();
        cout << "  " << name << ": токсичність зменшено вдвічі" << endl;
    }
};

class Crystal : public Ingredient
{
public:
    Crystal(const string& name) : Ingredient(name, 0)
    {}

    void addTo(Potion& potion) const override
    {
        potion.amplify();
        cout << "  " << name << ": усе в котлі посилено вдвічі" << endl;
    }
};

/*
   1. throwIn (і котел у рівні 2) знає лише про Ingredient. Як він «дізнається», що мухомор треба додати до токсичності,
      а траву — до лікування?
   4. throwIn приймає const Ingredient&. Що станеться, якщо зробити параметр Ingredient (за значенням)?
*/
void throwIn(Potion& potion, const Ingredient& ingredient)
{
    cout << "Кидаємо: " << ingredient.getName() << endl;    // невіртуальний метод через посилання
    ingredient.addTo(potion);                               // віртуальний — версія реального інгредієнта
}

// ==================== РІВЕНЬ 2. Котел ====================

class Cauldron
{
private:
    Ingredient** ingredients;
    int count;

public:
    Cauldron() : ingredients(nullptr), count(0)
    {}

    Cauldron(const Cauldron& other) = delete;
    Cauldron& operator=(const Cauldron& other) = delete;

    ~Cauldron()
    {
        clear();
    }

    bool add(Ingredient* ingredient)
    {
        if (ingredient == nullptr)
        {
            return false;
        }
        Ingredient** newIngredients = new Ingredient * [count + 1];
        for (int i = 0; i < count; i++)
        {
            newIngredients[i] = ingredients[i];
        }
        newIngredients[count] = ingredient;
        delete[] ingredients;             // старий масив покажчиків, а не інгредієнти
        ingredients = newIngredients;
        count++;
        return true;
    }

    // Котел володіє інгредієнтами: видаляє і кожен інгредієнт, і масив покажчиків
    void clear()
    {
        for (int i = 0; i < count; i++)
        {
            delete ingredients[i];        // віртуальний деструктор — видаляється реальний об'єкт
        }
        delete[] ingredients;
        ingredients = nullptr;
        count = 0;
    }

    void brew()
    {
        cout << "Котел кипить..." << endl;
        Potion potion;
        for (int i = 0; i < count; i++)
        {
            ingredients[i]->addTo(potion);   // котел не знає, що саме в нього кинули
        }
        potion.print();
        clear();                             // інгредієнти згоріли, котел знову порожній
    }
};

// ==================== РІВЕНЬ 3. Філософський камінь ====================

// final: камінь єдиний у своєму роді, різновидів у нього немає
//  6. (Рівень 3) Скільки рядків у Cauldron довелося змінити заради філософського каменя? Чому?
class PhilosopherStone final : public Ingredient
{
public:
    PhilosopherStone() : Ingredient("Філософський камінь", 0)
    {}

    void addTo(Potion& potion) const override
    {
        potion.transmute();
        cout << "  " << name << ": отруту перетворено на ліки" << endl;
    }
};

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    cout << "===== РІВЕНЬ 1 =====" << endl;
    Herb mint("М'ята", 30);
    Mushroom amanita("Мухомор", 40);
    Crystal crystal("Місячний кристал");
    /*
      7. Чи має сенс створювати об'єкт «просто Ingredient», як пісок у рівні 1? Як, на вашу думку, можна було б заборонити
         створювати такі об'єкти, щоб у котел потрапляли лише справжні інгредієнти?
    */
    Ingredient sand("Пісок", 0);
    Water water;

    cout << "--- Зілля 1: м'ята → кристал → мухомор ---" << endl;
    Potion first;
    throwIn(first, mint);
    throwIn(first, crystal);
    throwIn(first, amanita);
    first.print();                        // 60 / 40 → Зілля лікування

    cout << endl << "--- Зілля 2: мухомор → кристал → м'ята ---" << endl;
    Potion second;
    throwIn(second, amanita);
    throwIn(second, crystal);
    throwIn(second, mint);
    second.print();                       // 30 / 80 → Отрута: порядок має значення

    cout << endl << "--- Зілля 3: мухомор → пісок → вода ---" << endl;
    Potion third;
    throwIn(third, amanita);
    throwIn(third, sand);                 // версія Ingredient — нічого не змінює
    throwIn(third, water);
    third.print();                        // 0 / 20 → Отрута

    cout << endl << "===== РІВЕНЬ 2 =====" << endl;
    Cauldron cauldron;

    cauldron.add(new Herb("Женьшень", 50));
    cauldron.add(new Herb("М'ята", 30));
    cauldron.add(new Crystal("Місячний кристал"));
    cauldron.add(new Mushroom("Поганка", 20));
    cauldron.add(new Water());
    cauldron.brew();                      // 160 / 10 → Зілля лікування

    cout << endl;
    cauldron.add(new Herb("Женьшень", 50));
    cauldron.add(new Crystal("Місячний кристал"));
    cauldron.add(new Mushroom("Мухомор", 40));
    cauldron.add(new Crystal("Сонячний кристал"));
    cauldron.brew();                      // 200 / 80 → Котел вибухнув!

    cout << endl << "===== РІВЕНЬ 3 =====" << endl;
    cauldron.add(new Mushroom("Мухомор", 40));
    cauldron.add(new Mushroom("Поганка", 20));
    cauldron.add(new PhilosopherStone());
    cauldron.brew();                      // 60 / 0 → Зілля лікування

    return 0;
}

/*
  1. throwIn (і котел у рівні 2) знає лише про Ingredient. Як він «дізнається», що мухомор треба додати до токсичності,
     а траву — до лікування?
  2. getName() не віртуальний, а addTo() — віртуальний. Чому getName() не потрібно робити віртуальним?
     Що станеться з зіллям, якщо прибрати virtual в addTo() (і override у нащадках)?
  3. Що буде, якщо в Mushroom написати void addTo(Potion& potion) без const: а) з override; б) без override?
  4. throwIn приймає const Ingredient&. Що станеться, якщо зробити параметр Ingredient (за значенням)?
  5. (Рівень 2) Чому деструктор Ingredient віртуальний, хоча він нічого не робить? Де саме в програмі інгредієнт
     видаляється через покажчик на базовий клас?
  6. (Рівень 3) Скільки рядків у Cauldron довелося змінити заради філософського каменя? Чому?
  7. Чи має сенс створювати об'єкт «просто Ingredient», як пісок у рівні 1? Як, на вашу думку, можна було б заборонити
     створювати такі об'єкти, щоб у котел потрапляли лише справжні інгредієнти?

  ВІДПОВІДІ НА ЧАСТИНУ «ПОЯСНИ» (еталон для розбору)

  1. 

  2. 

  3. 

  4. 

  5. 

  6. 

  7.

*/

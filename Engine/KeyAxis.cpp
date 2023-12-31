#include "Input.h"

KeyAxis::KeyAxis() noexcept
{
    positiveKeys = new std::vector<KeyCode>();
    negativeKeys = new std::vector<KeyCode>();
}
KeyAxis::KeyAxis(KeyCode negative, KeyCode positive) noexcept
{
    positiveKeys = new std::vector<KeyCode>();
    negativeKeys = new std::vector<KeyCode>();

    BindPair(negative, positive);
}
KeyAxis::~KeyAxis() noexcept
{
    delete positiveKeys;
    delete negativeKeys;
}

float KeyAxis::value() const noexcept
{
    bool positive = false;
    bool negative = false;

    for (int i = 0; i < positiveKeys->size(); i++)
    {
        if (input->GetKeyDown((*positiveKeys)[i]))
        {
            positive = true;
            break;
        }
    }
    for (int i = 0; i < negativeKeys->size(); i++)
    {
        if (input->GetKeyDown((*negativeKeys)[i]))
        {
            negative = true;
            break;
        }
    }

    return (positive ? 1.0f : 0.0f) - (negative ? 1.0f : 0.0f);
}


void KeyAxis::BindPositive(KeyCode keyCode) noexcept
{
    positiveKeys->push_back(keyCode);
}
void KeyAxis::BindPositive(std::vector<KeyCode> keyCodes) noexcept
{
    for (int i = 0; i < keyCodes.size(); i++)
    {
        BindPositive(keyCodes[i]);
    }
}
void KeyAxis::BindNegative(KeyCode keyCode) noexcept
{
    negativeKeys->push_back(keyCode);
}
void KeyAxis::BindNegative(std::vector<KeyCode> keyCodes) noexcept
{
    for (int i = 0; i < keyCodes.size(); i++)
    {
        BindNegative(keyCodes[i]);
    }
}
void KeyAxis::BindPair(KeyCode negative, KeyCode positive) noexcept
{
    negativeKeys->push_back(negative);
    positiveKeys->push_back(positive);
}

void KeyAxis::UnbindPositive(KeyCode keyCode) noexcept
{
    for (int i = 0; i < positiveKeys->size(); i++)
    {
        if ((*negativeKeys)[i] == keyCode)
        {
            positiveKeys->erase(positiveKeys->begin() + i);
            break;
        }
    }
}
void KeyAxis::UnbindNegative(KeyCode keyCode) noexcept
{
    for (int i = 0; i < negativeKeys->size(); i++)
    {
        if ((*negativeKeys)[i] == keyCode)
        {
            negativeKeys->erase(negativeKeys->begin() + i);
            break;
        }
    }
}

void KeyAxis::ClearPositiveBinds() noexcept
{
    positiveKeys->clear();
}
void KeyAxis::ClearNegativeBinds() noexcept
{
    negativeKeys->clear();
}
void KeyAxis::ClearAllBinds() noexcept
{
    ClearPositiveBinds();
    ClearNegativeBinds();
}
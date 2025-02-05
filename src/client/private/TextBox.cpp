#include "TextBox.h"

TextInputBox& TextInputBox::Construct(Rectangle Box, Color BackgroundColor)
{
	this->Box = Box;
	this->BoxBackgroundColor = BackgroundColor;
	this->bUseBorder = false;
	this->TextPosition.x = this->Box.x + this->Box.width / 10;
	
	this->TextPosition.y = this->Box.y + this->Box.height / 2 - (FontSize/2);
	return *this;
}

TextInputBox& TextInputBox::Construct(float x, float y, float width, float height, Color BackgroundColor)
{
	Rectangle TempObj = { x,y, width, height };
	return Construct(TempObj, BackgroundColor);

}


TextInputBox& TextInputBox::UseBorder(bool UseBorder)
{
	return *this;
}

TextInputBox& TextInputBox::SetInitialText(std::string InitialText)
{
	this->Text = InitialText;
	return *this;
}

TextInputBox& TextInputBox::SetInitialText(char InitalText[51])
{
	strncpy(this->StringToHold, InitalText, sizeof(this->StringToHold) - 1);
	this->StringToHold[sizeof(this->StringToHold) - 1] = '\0';  // Ensure null-termination
	LetterCount = strlen(InitalText);
	return *this;
}

TextInputBox& TextInputBox::CanBeEdited(bool IsEditable)
{
	this->IsEditable = IsEditable;
	return *this;
}

TextInputBox& TextInputBox::ChangeMaxChars(int MaxCharacters)
{
	MaxChars = MaxCharacters;
	return *this;
}

TextInputBox& TextInputBox::EreaseText()
{
	LetterCount = 0;
	StringToHold[LetterCount] = '\0';
	return *this;
}

TextInputBox& TextInputBox::UpdateTextOffset(Vector2 NewTextPosition)
{
	this->TextOffset = NewTextPosition;
	return *this;
}

TextInputBox& TextInputBox::UpdateTextPosition()
{
	this->TextPosition.x = this->Box.x + this->Box.width / 10;

	this->TextPosition.y = this->Box.y + this->Box.height / 2 - (FontSize / 2);
	return *this;
}

TextInputBox& TextInputBox::UpdateFontSize(int NewFontSize)
{
	FontSize = NewFontSize;
	return *this;
}

TextInputBox& TextInputBox::UpdateFontColor(Color NewFontColor)
{
	FontColor = NewFontColor;
	return *this;
}

TextInputBox& TextInputBox::UpdatePosition(Vector2 NewPosition)
{
	this->Box.x = NewPosition.x;
	this->Box.y = NewPosition.y;
	return *this;
}

TextInputBox& TextInputBox::UpdateSize(Vector2 NewSize)
{
	this->Box.width = NewSize.x;
	this->Box.height = NewSize.y;
	return *this;
}

TextInputBox& TextInputBox::UpdateColor(Color NewColor)
{
	this->BoxBackgroundColor = NewColor;
	return *this;
}

TextInputBox& TextInputBox::UpdateTextShown(std::string Text)
{
	this->Text = Text;
	return *this;
}

TextInputBox& TextInputBox::UpdateBorder(int Thickness, Color BorderColor)
{
	return *this;
}

bool TextInputBox::bIsFocused(Vector2 MousePosition)
{
	return CheckCollisionPointRec(MousePosition, this->Box);
}




void TextInputBox::Update()
{
	DrawRectangle(this->Box.x, this->Box.y, this->Box.width, this->Box.height, this->BoxBackgroundColor);
	


	if (bIsFocused(GetMousePosition()) && this->IsEditable)
	{
		int key = GetCharPressed();

		if (IsKeyPressed(KEY_BACKSPACE))
		{
			LetterCount--;

			if (LetterCount < 0)
			{
				LetterCount = 0;
			}
			StringToHold[LetterCount] = '\0';
		}

		else if (strlen(StringToHold) < MaxChars && key > 0)
		{
			StringToHold[LetterCount] = (char)key;
			StringToHold[LetterCount + 1] = '\0';
			LetterCount++;
		}
	}

	if (IsEditable)
	DrawText(this->StringToHold, TextPosition.x + TextOffset.x, TextPosition.y + TextOffset.y, FontSize, FontColor);

	else
	DrawText(this->Text.c_str(), TextPosition.x + TextOffset.x, TextPosition.y + TextOffset.y, FontSize, FontColor);
}

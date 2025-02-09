#include "TextBox.h"

TextInputBox& TextInputBox::Construct(Rectangle Box, Color BackgroundColor)
{
	this->Box = Box;
	this->BoxBackgroundColor = BackgroundColor;
	this->bUseBorder = false;
	this->TextPosition.x = this->Box.x;
	
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

TextInputBox& TextInputBox::UseWordWrap()
{
	bContainsText = true;
	bWordWrap = true;
	return *this;
}

TextInputBox& TextInputBox::UseContains()
{
	bContainsText = true;
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
	unsigned int Calculate = MeasureText(StringToHold, FontSize);
	this->TextPosition.x = this->Box.x;

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
	if (bGrowBoxToText)
	{
		// TODO
	}
	else
	{
		DrawRectangle(this->Box.x, this->Box.y, this->Box.width, this->Box.height, this->BoxBackgroundColor);
	}

	

	if (bIsFocused(GetMousePosition()) && this->IsEditable)
	{
		int PressedKey = GetCharPressed();

		// ########################
		// Deleting Logic
		// ########################
		if (IsKeyPressed(KEY_BACKSPACE))
		{
			if (LetterCount > 0)
			{
				LetterCount--;
				StringToHold[LetterCount] = '\0';
			}

			DelayTimer = 0.0f;
			RepeatTimer = 0.0f;
		}
		else if (IsKeyDown(KEY_BACKSPACE))
		{
			float DeltaTime = GetFrameTime();
			DelayTimer += DeltaTime;

			if (DelayTimer >= Delay)
			{
				// We Delete Characters depending on the Interval defined
				RepeatTimer += DeltaTime;
				if (RepeatTimer >= RepeatInterval)
				{
					if (LetterCount > 0)
					{
						LetterCount--;
						StringToHold[LetterCount] = '\0';
					}

					// Reset the repeat timer for the next deletion.
					RepeatTimer = 0.0f;
				}
			}
		}
		else
		{
			DelayTimer = 0.0f;
			RepeatTimer = 0.0f;
		}

		// ########################
		// Deleting Logic Finished
		// ########################

		// ########################
		// Appending Characters Logic
		// ########################

		if (strlen(StringToHold) < MaxChars && PressedKey > 0)
		{
			std::cout << LetterCount << std::endl;
			std::cout << MeasureText(StringToHold, FontSize) << " Vs: " << Box.width << std::endl;
			if (MeasureText(StringToHold, FontSize) > Box.width && bContainsText)
			{
				if (bWordWrap)
				{
					/*
					* Basicially we just insert and resize an array here
					*/
						std::string TextBuffer;
						// Assigning all but the first space from the last time we did a space to a buffer
						TextBuffer.append(&StringToHold[LastSpaceIndex] + 1);
						// then we do a Line Break where the last space was made
						StringToHold[LastSpaceIndex-1] = '\n';
						// Adapt the Lettercount accordingly (Was the letter count where we made the last letter - the delta of the lastSpaceIndex
						LetterCount = LetterCount - (LetterCount - LastSpaceIndex);

						// then we add the stuff from the Textbuffer back
						for (int i = 0; i < TextBuffer.size(); i++)
						{
							LetterCount++;
							StringToHold[LetterCount] = TextBuffer[i];
						}

						// then we assign the key we actually pressed too, as we never added it up the actual string
						StringToHold[LetterCount + 1] = (char)PressedKey;
						StringToHold[LetterCount + 2] = '\0';
						LetterCount++;
						std::cout << "After WordWrap: " << LetterCount << std::endl;
				}

				// Just Letter Wrap no WordWrap
				else
				{
					StringToHold[LetterCount] = '\n';
					StringToHold[LetterCount + 1] = (char)PressedKey;
					StringToHold[LetterCount + 2] = '\0';
					LetterCount++;
				}
				LetterCount++;
			}

			// Normal appending
			else
			{

				StringToHold[LetterCount] = (char)PressedKey;
				StringToHold[LetterCount + 1] = '\0';
				LetterCount++;
			}

			// Saving the Index of the Space for the next case
			if (PressedKey == 32)
			{
				BackupSpaceIndex = LastSpaceIndex;
				LastSpaceIndex = LetterCount;
			}
			
		}
	}


	if(IsEditable)
	{
		DrawText(this->StringToHold, TextPosition.x + TextOffset.x, TextPosition.y + TextOffset.y, FontSize, FontColor);
	}
	
	
	else
	{
		DrawText(this->Text.c_str(), TextPosition.x + TextOffset.x, TextPosition.y + TextOffset.y, FontSize, FontColor);
	}
	
}

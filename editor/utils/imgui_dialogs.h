#pragma once

#include "utils/imgui_buttons.h"

#include "extras/IconsFontAwesome6.h"

#include <string>
#include <functional>

namespace ImGui
{
    enum class DialogResult
    {
        None,
        Accept,
        Decline,
        Cancel,
    };

    DialogResult DialogButtons(bool allowAccept = true, const char* accept = "OK", const char* cancel = "Cancel", const char* decline = nullptr);

    void SetDialogAcceptKey(int key);

    class DialogBox
    {
    public:
        virtual void OnCreate() {}
        virtual void OnShow() {}
        virtual void OnResult(DialogResult result) {}
        virtual void OnClose() {}

        virtual bool AllowAccept() { return true; }
        virtual const char* GetDisplayName() { return DisplayName.c_str(); }

        ImVec2 InitalSize{ 300,200 };

        const char* AcceptButton = "OK";
        const char* CancelButton = "Cancel";
        const char* DeclineButton = nullptr;

        std::string Icon;
        std::string DisplayName = "Default Dialog";
        std::string ImGuiName;

        size_t ID = 0;
    };

    DialogBox* ShowDialog(DialogBox* dialog, std::function<void(DialogBox*)> destoryFunctiom = nullptr);

    size_t UpdateDialogs();


    class MessageBoxDialog : public DialogBox
    {
    public:
        MessageBoxDialog(const std::string& title, const std::string& message)
        {
            CancelButton = nullptr;
            Icon = ICON_FA_EXCLAMATION;
            DisplayName = title;
            Message = message;
        }

        inline void OnShow() override
        {
            ImGui::TextUnformatted(Message.c_str());
        }
        std::string Message;
    };

    class YesNoDialog : public MessageBoxDialog
    {
    public:
        YesNoDialog(const std::string& title, const std::string& message, std::function<void(YesNoDialog*)> yes = nullptr, std::function<void(YesNoDialog*)> no = nullptr)
            : MessageBoxDialog(title,message)
        {
            AcceptButton = "Yes";
            DeclineButton = "No";
            Icon = ICON_FA_QUESTION;
            YesCallback = yes;
            NoCallback = no;
        }

        inline void OnResult(DialogResult result) override
        {
            if (result == DialogResult::Accept && YesCallback)
                YesCallback(this);

            if (result != DialogResult::Accept && NoCallback)
                NoCallback(this);
        }

        std::function<void(YesNoDialog*)> YesCallback;
        std::function<void(YesNoDialog*)> NoCallback;
    };

    class YesNoCancelDialog : public MessageBoxDialog
    {
    public:
        YesNoCancelDialog(const std::string& title, const std::string& message, std::function<void(YesNoCancelDialog*)> yes, std::function<void(YesNoCancelDialog*)> no, std::function<void(YesNoCancelDialog*)> cancel)
            : MessageBoxDialog(title, message) 
        {
            CancelButton = "Cancel";
            AcceptButton = "Yes";
            DeclineButton = "No";
            Icon = ICON_FA_QUESTION;
            YesCallback = yes;
            NoCallback = no;
            CancelCallback = cancel;
        }

        inline void OnResult(DialogResult result) override
        {
            if (result == DialogResult::Accept && YesCallback)
                YesCallback(this);

            if (result != DialogResult::Accept && NoCallback)
                NoCallback(this);
        }

        std::function<void(YesNoCancelDialog*)> YesCallback;
        std::function<void(YesNoCancelDialog*)> NoCallback;
        std::function<void(YesNoCancelDialog*)> CancelCallback;
    };

    class CallbackDialog : public DialogBox
    {
    public:
        CallbackDialog(const std::string& title, const std::string icon)
        {
            Icon = icon;
            DisplayName = title;
        }

        void* DataPtr = nullptr;

        std::function<void(CallbackDialog*)> CreateCallback;
        std::function<void(CallbackDialog*)> ShowCallback;
        std::function<void(DialogResult, CallbackDialog*)> ResultCallback;
        std::function<void(CallbackDialog*)> CloseCallback;

        std::function<bool(CallbackDialog*)> AllowAcceptCallback;

        inline void OnCreate() override
        {
            if (CreateCallback != nullptr)
                CreateCallback(this);
        }

        inline void OnShow() override
        {
            if (ShowCallback != nullptr)
                ShowCallback(this);
        }

        inline void OnClose() override
        {
            if (CloseCallback != nullptr)
                CloseCallback(this);
        }

        inline bool AllowAccept() override 
        {
            if (AllowAcceptCallback != nullptr)
                return AllowAcceptCallback(this);

            return true; 
        }

        inline void OnResult(DialogResult result) override
        {
            if (ResultCallback != nullptr)
                ResultCallback(result, this);
        }

        static CallbackDialog* Show(const std::string& title, const std::string icon, std::function<void(CallbackDialog*)> showCallback = nullptr, std::function<void(DialogResult, CallbackDialog*)> resultCallback = nullptr, void* dataPtr = nullptr)
        {
            CallbackDialog* dlog = new CallbackDialog(title, icon);
            dlog->ShowCallback = showCallback;
            dlog->ResultCallback = resultCallback;
            dlog->DataPtr = dataPtr;
            ImGui::ShowDialog(dlog);
            return dlog;
        }
    };
}
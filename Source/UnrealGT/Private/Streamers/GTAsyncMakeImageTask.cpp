// Fill out your copyright notice in the Description page of Project Settings.

#include "GTAsyncMakeImageTask.h"

#include <IImageWrapper.h>
#include <IImageWrapperModule.h>
#include <MemoryWriter.h>
#include <ModuleManager.h>
#include "GTImageGeneratorBase.h"
#include <Async.h>

FGTAsyncMakeImageTask::FGTAsyncMakeImageTask(UGTImageGeneratorBase* SourceComponent,
                                             const FGTImage& Image,
                                             EGTImageFileFormat ImageFormat, bool bWriteAlpha, FDateTime TimeStamp)
    : SourceComponent(SourceComponent)
    , Image(Image)
    , ImageFormat(ImageFormat)
    , TimeStamp(TimeStamp)
    , bWriteAlpha(bWriteAlpha)
{
    FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
}

void FGTAsyncMakeImageTask::DoWork()
{
    IImageWrapperModule& ImageWrapperModule = FModuleManager::GetModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TArray<uint8> ImgData;
    switch (ImageFormat)
    {
        case EGTImageFileFormat::PNG:
        {
            TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
            ImageWrapper->SetRaw(Image.Pixels.GetData(), Image.Pixels.GetAllocatedSize(), Image.Width, Image.Height, ERGBFormat::BGRA, 8);
            ImgData = ImageWrapper->GetCompressed(100);
            break;
        }
        case EGTImageFileFormat::BMP:
        {
            //TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
            //ImageWrapper->SetRaw(Image.Pixels.GetData(), Image.Pixels.GetAllocatedSize(), Image.Width, Image.Height, ERGBFormat::BGRA, 8);
            //ImgData = ImageWrapper->GetCompressed(100);

            // bitmap support in imagewrapper is buggy
            int Width = Image.Width;
            int Height = Image.Height;

            FColor* Data = Image.Pixels.GetData();
            
			// TODO configure?
            FIntRect SubRectangle(0, 0, Width, Height);
            
            FMemoryWriter Ar(ImgData);
            
#if PLATFORM_SUPPORTS_PRAGMA_PACK
            #pragma pack(push, 1)
#endif
            struct BITMAPFILEHEADER
            {
                uint16 bfType GCC_PACK(1);
                uint32 bfSize GCC_PACK(1);
                uint16 bfReserved1 GCC_PACK(1);
                uint16 bfReserved2 GCC_PACK(1);
                uint32 bfOffBits GCC_PACK(1);
            } FH;
            struct BITMAPINFOHEADER
            {
                uint32 biSize GCC_PACK(1);
                int32 biWidth GCC_PACK(1);
                int32 biHeight GCC_PACK(1);
                uint16 biPlanes GCC_PACK(1);
                uint16 biBitCount GCC_PACK(1);
                uint32 biCompression GCC_PACK(1);
                uint32 biSizeImage GCC_PACK(1);
                int32 biXPelsPerMeter GCC_PACK(1);
                int32 biYPelsPerMeter GCC_PACK(1);
                uint32 biClrUsed GCC_PACK(1);
                uint32 biClrImportant GCC_PACK(1);
            } IH;
            struct BITMAPV4HEADER
            {
                uint32 bV4RedMask GCC_PACK(1);
                uint32 bV4GreenMask GCC_PACK(1);
                uint32 bV4BlueMask GCC_PACK(1);
                uint32 bV4AlphaMask GCC_PACK(1);
                uint32 bV4CSType GCC_PACK(1);
                uint32 bV4EndpointR[3] GCC_PACK(1);
                uint32 bV4EndpointG[3] GCC_PACK(1);
                uint32 bV4EndpointB[3] GCC_PACK(1);
                uint32 bV4GammaRed GCC_PACK(1);
                uint32 bV4GammaGreen GCC_PACK(1);
                uint32 bV4GammaBlue GCC_PACK(1);
            } IHV4;
#if PLATFORM_SUPPORTS_PRAGMA_PACK
            #pragma pack(pop)
#endif
            
            uint32 BytesPerPixel = bWriteAlpha ? 4 : 3;
            uint32 BytesPerLine = Align(Width * BytesPerPixel, 4);
            
            uint32 InfoHeaderSize = sizeof(BITMAPINFOHEADER) + (bWriteAlpha ? sizeof(BITMAPV4HEADER) : 0);
            
            // File header.
            FH.bfType = INTEL_ORDER16((uint16)('B' + 256 * 'M'));
            FH.bfSize = INTEL_ORDER32((uint32)(sizeof(BITMAPFILEHEADER) + InfoHeaderSize + BytesPerLine * Height));
            FH.bfReserved1 = INTEL_ORDER16((uint16)0);
            FH.bfReserved2 = INTEL_ORDER16((uint16)0);
            FH.bfOffBits = INTEL_ORDER32((uint32)(sizeof(BITMAPFILEHEADER) + InfoHeaderSize));
            Ar.Serialize(&FH, sizeof(FH));
            
            // Info header.
            IH.biSize = INTEL_ORDER32((uint32)InfoHeaderSize);
            IH.biWidth = INTEL_ORDER32((uint32)Width);
            IH.biHeight = INTEL_ORDER32((uint32)Height);
            IH.biPlanes = INTEL_ORDER16((uint16)1);
            IH.biBitCount = INTEL_ORDER16((uint16)BytesPerPixel * 8);
            if (bWriteAlpha)
            {
                IH.biCompression = INTEL_ORDER32((uint32)3);  // BI_BITFIELDS
            }
            else
            {
                IH.biCompression = INTEL_ORDER32((uint32)0);  // BI_RGB
            }
            IH.biSizeImage = INTEL_ORDER32((uint32)BytesPerLine * Height);
            IH.biXPelsPerMeter = INTEL_ORDER32((uint32)0);
            IH.biYPelsPerMeter = INTEL_ORDER32((uint32)0);
            IH.biClrUsed = INTEL_ORDER32((uint32)0);
            IH.biClrImportant = INTEL_ORDER32((uint32)0);
            Ar.Serialize(&IH, sizeof(IH));
            
            // If we're writing alpha, we need to write the extra portion of the V4 header
            if (bWriteAlpha)
            {
                IHV4.bV4RedMask = INTEL_ORDER32((uint32)0x00ff0000);
                IHV4.bV4GreenMask = INTEL_ORDER32((uint32)0x0000ff00);
                IHV4.bV4BlueMask = INTEL_ORDER32((uint32)0x000000ff);
                IHV4.bV4AlphaMask = INTEL_ORDER32((uint32)0xff000000);
                IHV4.bV4CSType = INTEL_ORDER32((uint32)'Win ');
                IHV4.bV4GammaRed = INTEL_ORDER32((uint32)0);
                IHV4.bV4GammaGreen = INTEL_ORDER32((uint32)0);
                IHV4.bV4GammaBlue = INTEL_ORDER32((uint32)0);
                Ar.Serialize(&IHV4, sizeof(IHV4));
            }
            
            // Colors.
            for (int32 i = SubRectangle.Max.Y - 1; i >= SubRectangle.Min.Y; i--)
            {
                for (int32 j = SubRectangle.Min.X; j < SubRectangle.Max.X; j++)
                {
                    Ar.Serialize((void*)&Data[i * Width + j].B, 1);
                    Ar.Serialize((void*)&Data[i * Width + j].G, 1);
                    Ar.Serialize((void*)&Data[i * Width + j].R, 1);
            
                    if (bWriteAlpha)
                    {
                        Ar.Serialize((void*)&Data[i * Width + j].A, 1);
                    }
                }
            
                // Pad each row's length to be a multiple of 4 bytes.
            
                for (uint32 PadIndex = Width * BytesPerPixel; PadIndex < BytesPerLine; PadIndex++)
                {
                    uint8 B = 0;
                    Ar.Serialize(&B, 1);
                }
            }
            break;
        }
    }

    UGTImageGeneratorBase* SourceComponentLocal = SourceComponent;
    FDateTime TimeStamptLocal = TimeStamp;

    // TODO figure out if we can pass ImgData by ref
    // probably not because this thread could be dead by the time the lambda is called on the gamethread
    AsyncTask(ENamedThreads::GameThread, [SourceComponentLocal, ImgData, TimeStamptLocal]() {
        if (SourceComponentLocal && SourceComponentLocal->IsValidLowLevelFast())
        {
            SourceComponentLocal->DataReadyDelegate.Broadcast(ImgData, TimeStamptLocal);
        }
    });

}

TStatId FGTAsyncMakeImageTask::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(FGTMakeImageTask, STATGROUP_ThreadPoolAsyncTasks);
}

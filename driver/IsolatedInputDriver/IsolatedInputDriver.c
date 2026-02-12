#include "IsolatedInputDriver.h"

_Use_decl_annotations_
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;

    WDF_DRIVER_CONFIG_INIT(&config, IsoEvtDeviceAdd);

    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.EvtCleanupCallback = IsoEvtDriverContextCleanup;

    status = WdfDriverCreate(DriverObject, RegistryPath, &attributes, &config, WDF_NO_HANDLE);
    return status;
}

_Use_decl_annotations_
NTSTATUS IsoEvtDeviceAdd(WDFDRIVER Driver, PWDFDEVICE_INIT DeviceInit)
{
    UNREFERENCED_PARAMETER(Driver);

    WdfFdoInitSetFilter(DeviceInit);

    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

    WDFDEVICE device;
    NTSTATUS status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    PDEVICE_CONTEXT context = IsoGetDeviceContext(device);
    context->AssignedSessionId = 0;
    context->IsolationEnabled = FALSE;

    WDF_IO_QUEUE_CONFIG queueConfig;
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchParallel);
    queueConfig.EvtIoDeviceControl = IsoEvtIoDeviceControl;

    return WdfIoQueueCreate(device, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, WDF_NO_HANDLE);
}

_Use_decl_annotations_
VOID IsoEvtIoDeviceControl(
    WDFQUEUE Queue,
    WDFREQUEST Request,
    size_t OutputBufferLength,
    size_t InputBufferLength,
    ULONG IoControlCode)
{
    UNREFERENCED_PARAMETER(OutputBufferLength);

    WDFDEVICE device = WdfIoQueueGetDevice(Queue);
    PDEVICE_CONTEXT context = IsoGetDeviceContext(device);
    NTSTATUS status = STATUS_SUCCESS;
    size_t bytesReturned = 0;

    switch (IoControlCode) {
    case IOCTL_ISO_SET_SESSION:
    {
        if (InputBufferLength < sizeof(ISO_SESSION_ASSIGNMENT)) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        PISO_SESSION_ASSIGNMENT assignment = NULL;
        status = WdfRequestRetrieveInputBuffer(Request, sizeof(ISO_SESSION_ASSIGNMENT), (PVOID*)&assignment, NULL);
        if (!NT_SUCCESS(status)) {
            break;
        }

        context->AssignedSessionId = assignment->SessionId;
        context->IsolationEnabled = TRUE;
        break;
    }
    case IOCTL_ISO_GET_STATUS:
    {
        if (OutputBufferLength < sizeof(ISO_DEVICE_STATUS)) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        PISO_DEVICE_STATUS output = NULL;
        status = WdfRequestRetrieveOutputBuffer(Request, sizeof(ISO_DEVICE_STATUS), (PVOID*)&output, NULL);
        if (!NT_SUCCESS(status)) {
            break;
        }

        output->SessionId = context->AssignedSessionId;
        output->IsolationEnabled = context->IsolationEnabled;
        bytesReturned = sizeof(ISO_DEVICE_STATUS);
        break;
    }
    case IOCTL_ISO_ENABLE:
        context->IsolationEnabled = TRUE;
        break;
    case IOCTL_ISO_DISABLE:
        context->IsolationEnabled = FALSE;
        break;
    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    WdfRequestCompleteWithInformation(Request, status, bytesReturned);
}

_Use_decl_annotations_
VOID IsoEvtDriverContextCleanup(WDFOBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
}

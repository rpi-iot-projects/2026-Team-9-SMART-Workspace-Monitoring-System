from django.shortcuts import render
from .models import DistanceReading, WeightReading, RFIDReading
from django.http import StreamingHttpResponse
from datetime import timedelta
import time 
import json

# Create your views here.
def dashboard_view(request):
    return render(request, "web/home.html")

# Return request data 
def sse_data(request):
    def event_stream():
        last_rfid_id = None
        last_distance_id = None
        last_weight_id = None

        while True:
            distance_qs = DistanceReading.objects.all()
            weight_qs = WeightReading.objects.all()
            rfid_qs = RFIDReading.objects.all()

            # Grab last 20 entries of db
            if last_distance_id:
                distance_qs = distance_qs.filter(id__gt=last_distance_id)
            else:
                distance_qs = distance_qs.order_by("-id")[:20][::-1]

            if last_weight_id:
                weight_qs = weight_qs.filter(id__gt=last_weight_id)
            else:
                weight_qs = weight_qs.order_by("-id")[:20][::-1]

            if last_rfid_id:
                rfid_qs = rfid_qs.filter(id__gt=last_rfid_id)
            else:
                rfid_qs = rfid_qs.order_by("-id")[:20][::-1]

            distances = list(distance_qs)
            weights = list(weight_qs)
            rfids = list(rfid_qs)

            if distances:
                last_distance_id = distances[-1].id
            if weights:
                last_weight_id = weights[-1].id
            if rfids:
                last_rfid_id = rfids[-1].id
            # Store each data type separately
            data = {
                "distances": [
                    {
                        "value": d.value,
                        "time": (d.timestamp - timedelta(hours=4)).strftime("%I:%M:%S %p")
                    } for d in distances
                ],
                "weights": [
                    {
                        "left": w.left,
                        "right": w.right,
                        "time": (w.timestamp - timedelta(hours=4)).strftime("%I:%M:%S %p")
                    } for w in weights
                ],
                "rfids": [
                    {
                        "value": r.mode, 
                        "time": (r.timestamp - timedelta(hours=4)).strftime("%I:%M:%S %p")
                    } for r in rfids
                ]
            }
            yield f"data: {json.dumps(data)}\n\n"
            time.sleep(0.2)
    return StreamingHttpResponse(event_stream(), content_type="text/event-stream")
from django.db import models

# Create your models here.
class DistanceReading(models.Model):
    value = models.FloatField(default = 0.0)
    timestamp = models.DateTimeField(auto_now_add=True)

class WeightReading(models.Model):
    left = models.FloatField(default = 0.0)
    right = models.FloatField(default = 0.0)
    timestamp = models.DateTimeField(auto_now_add=True)

class RFIDReading(models.Model):
    mode = models.CharField(max_length=50)
    timestamp = models.DateTimeField(auto_now_add=True)
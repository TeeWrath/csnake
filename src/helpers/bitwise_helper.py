# Bitwise operation helpers for better readability
def bitmask(size):
    """Create a bitmask of given size"""
    return (1 << size) - 1

def get_bit(value, position):
    """Get bit at given position"""
    return (value >> position) & 1

def set_bit(value, position):
    """Set bit at given position"""
    return value | (1 << position)

def clear_bit(value, position):
    """Clear bit at given position"""
    return value & ~(1 << position)

def toggle_bit(value, position):
    """Toggle bit at given position"""
    return value ^ (1 << position)